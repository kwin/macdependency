//
//  MachOModel.m
//  MacDependency
//
//  Created by Konrad Windszus on 13.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import "MachOModel.h"
#import "MyDocument.h"

#import "ConversionStdString.h"
#import "VersionFormatter.h"
#import "ArchitectureModel.h"

#include "MachO/machoexception.h"
#include "MachO/machoheader.h"
#include "MachO/symboltablecommand.h"


// declare private methods
@interface MachOModel ()
- (void) initChildren;
- (void) checkConstraints:(DylibCommand*) dylibId;
- (void) setStateWithWarning:(BOOL)isWarning;
- (id) initWithFile:(MachO*)file command:(DylibCommand*)command document:(MyDocument*)document parent:(MachOModel*)parent architecture:(MachOArchitecture*)architecture;
@end

@implementation MachOModel

- (id) initWithFile:(MachO*)aFile document:(MyDocument*)aDocument architecture:(MachOArchitecture*)anArchitecture loadChildren:(BOOL)loadChildren {
	state = StateNormal;
	[self initWithFile:aFile command:nil document:aDocument parent:nil architecture:anArchitecture];
	if (loadChildren) {
		[self initChildren];
	}
	return self;
}

// called by initChildren (calls initWithFile internally)
- (id) initWithFilename:(std::string&)filename command:(DylibCommand*)aCommand document:(MyDocument*)aDocument parent:(MachOModel*)aParent {
	BOOL isWeakReference = (command && !command->isNecessary());
    MachO* aFile;
    MachOArchitecture* anArchitecture;
    state = StateNormal;
	try {
		aFile = [aDocument cache]->getFile(filename, aParent->file); // throws exception in case file is not found
		anArchitecture = aFile->getCompatibleArchitecture(aParent->architecture);
		if (!anArchitecture) {
			[self setStateWithWarning:isWeakReference];
			NSString* log = [NSString stringWithFormat:NSLocalizedString(@"ERR_ARCHITECTURE_MISMATCH", nil), filename.c_str(), [parent name]];
			[aDocument appendLogLine:log withModel:self state:state];
		}
		
	}  catch (MachOException& exc) {
		[self setStateWithWarning:isWeakReference];
		NSString* log = [NSString stringWithStdString:exc.getCause()];
		[aDocument appendLogLine:log withModel:self state:state];
		// distinguish between weak and strong. In both cases append to tree with a status color				
	}
	[aDocument incrementNumDependencies];
	[self initWithFile:aFile command:aCommand document:aDocument parent:aParent architecture:anArchitecture];
	return self;
}				 

// private
- (id) initWithFile:(MachO*)aFile command:(DylibCommand*)aCommand document:(MyDocument*)aDocument parent:(MachOModel*)aParent architecture:(MachOArchitecture*)anArchitecture {
	if (self = [super init]) {
		document = aDocument;
		parent = aParent;
		command = aCommand;
		file = aFile;
		children = nil;
		architecture = anArchitecture;
		if (architecture) {
			DylibCommand* dylibId = architecture->getDynamicLibIdCommand();
			if (dylibId && command) {
				[self checkConstraints:dylibId];
			}
		} 
		
	} return self;
}

- (void)dealloc {
	[children release];
	[super dealloc];
}

- (void) setStateWithWarning:(BOOL)isWarning {
	State newState = StateError;
	if (isWarning) {
		newState = StateWarning;
	}
	
	if (self->state < newState) {
		self->state = newState;
	}
}

- (id) parent {
	return parent;
}

- (void) checkConstraints:(DylibCommand*) dylibId {
	// check version information (only compatible version information is relevant)
	unsigned int minVersion = dylibId->getCompatibleVersion();
	unsigned int requestedMinVersion = command->getCompatibleVersion();
	unsigned int requestedMaxVersion = command->getCurrentVersion();
	
	VersionFormatter* versionFormatter = [[VersionFormatter alloc] init];
	
	BOOL isWeakReference = (command && !command->isNecessary());
	
	NSString* log;
	// check minimum version
	if (minVersion != 0 && requestedMinVersion != 0 && minVersion < requestedMinVersion) {
		[self setStateWithWarning:isWeakReference];
		log = [NSString stringWithFormat:NSLocalizedString(@"ERR_MINIMUM_VERSION", nil), parent->command->getName().c_str(), [parent name], [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:requestedMinVersion]], [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:minVersion]]];
		[document appendLogLine:log withModel:self state:state];
		
	}
	
	// extended checks which are currently not done by dyld
	
	// check maximum version
	if (minVersion != 0 && requestedMaxVersion != 0 && minVersion > requestedMaxVersion) {
		[self setStateWithWarning:YES];
		log = [NSString stringWithFormat:NSLocalizedString(@"ERR_MAXIMUM_VERSION", nil), command->getName().c_str(), [parent name], [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:requestedMaxVersion]], [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:minVersion]]];
		[document appendLogLine:log withModel:self state:state];
		
	}
	
	// check names
	if (dylibId->getName() != command->getName()) {
		[self setStateWithWarning:YES];
		log = [NSString stringWithFormat:NSLocalizedString(@"ERR_NAME_MISMATCH", nil), command->getName().c_str(), [parent name], dylibId->getName().c_str()];
		[document appendLogLine:log withModel:self state:state];
	}
	
	// TODO: check for relative paths, since they can lead to problems
	[versionFormatter release];
} 

- (NSArray*)children {
	if (children == nil) {
		[self initChildren];
	}
	return children;
}

- (void) initChildren {
	// TODO: tweak capacity
	children = [NSMutableArray arrayWithCapacity:20];
	[children retain];
	if (architecture) {
		std::string workingDirectory = [[document workingDirectory] stdString];
		for (MachOArchitecture::LoadCommandsConstIterator it = architecture->getLoadCommandsBegin();
			 it != architecture->getLoadCommandsEnd();
			 ++it) {
			
			LoadCommand* childLoadCommand = (*it);
			// check if it is dylibcommand
			DylibCommand* dylibCommand = dynamic_cast<DylibCommand*> (childLoadCommand);
			if (dylibCommand != NULL && !dylibCommand->isId()) {
				std::string filename = architecture->getResolvedName(dylibCommand->getName(), workingDirectory);
				
				MachOModel* child = [MachOModel alloc];
				[children addObject:child]; // must add children before initializing them, because in init we rely on parent children being correct					 
				[child initWithFilename:filename command:dylibCommand document:document parent:self];
				[child release];
			}
		}
	}
}

- (BOOL)isLeaf {
	[self children];
	if ([children count] == 0)
		return YES;
	return NO;
}

- (NSColor*) textColor {
	NSColor* color;
	switch(state) {
		case StateWarning:
			color = [NSColor blueColor];
			break;
		case StateError:
			color = [NSColor redColor];
			break;
		default:
			color= [NSColor blackColor];
	}
	return color;
}

- (NSString*) filename {
	std::string filename;
	if (file)
		filename = file->getFileName();
	return [NSString stringWithStdString:filename];
}

- (NSString*) version {
	if (file) {
		return [NSString stringWithStdString:file->getVersion()];
	}
	return @"";
}

- (NSNumber*) size {
	NSNumber* size = [NSNumber alloc];
	if (file)
		[size initWithUnsignedLongLong:file->getSize()];
	else 
		[size initWithInt:0];
	return size;
}

- (NSString*) name {
	std::string name;
	if (command) {
		name = command->getName();
	} else if (file) {
		name = file->getName();
	}
	return [NSString stringWithStdString:name];
}

- (NSNumber*) currentVersion {
	if (command) {
		return [NSNumber numberWithUnsignedInt:command->getCurrentVersion()];
	}
	return [NSNumber numberWithUnsignedInt:0];
}

- (NSNumber*) compatibleVersion {
	if (command) {
		return [NSNumber numberWithUnsignedInt:command->getCompatibleVersion()];
	}
	return [NSNumber numberWithUnsignedInt:0];
}

- (NSDate*) lastModificationTime {
	if (file)
		return [NSDate dateWithTimeIntervalSince1970:file->getLastModificationTime()];
	return NULL;
}

- (NSString*) dependencyType {
	NSString* type;
	if (!command) {
		return @"";
	}
	switch(command->getType()) {
			
		case DylibCommand::DependencyWeak:		
			type = NSLocalizedString(@"DEPENDENCY_TYPE_WEAK", nil);
			break;
		case DylibCommand::DependencyDelayed:   
			type = NSLocalizedString(@"DEPENDENCY_TYPE_DELAYED", nil);
			break;
		case DylibCommand::DependencyNormal:  		
			type = NSLocalizedString(@"DEPENDENCY_TYPE_NORMAL", nil);
			break;
		default:
			type = NSLocalizedString(@"UNDEFINED", @"Unknown");
	}
	return type;
}

- (NSArray*) architectures {
	NSMutableArray* architectures = [NSMutableArray arrayWithCapacity:4];
	
	if (file) {
		for (MachO::MachOArchitecturesIterator iter = file->getArchitecturesBegin(); iter != file->getArchitecturesEnd(); iter++) {
			// create model for architecture
			ArchitectureModel* currentArchitecture = [[ArchitectureModel alloc]initWithArchitecture:(*iter) file:file document:document isRoot:NO];
			// correct order (current architecture should have first index)
			if ((*iter) == architecture) {
				[architectures insertObject:currentArchitecture atIndex:0]; // insert at beginning
			} else {
				[architectures addObject:currentArchitecture]; // insert at end
			}
			
			[currentArchitecture release];
		}
	}
	return  architectures;
}
@end
