//
//  ArchitectureModel.m
//  MacDependency
//
//  Created by Konrad Windszus on 22.08.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import "ArchitectureModel.h"
#import "VersionFormatter.h"
#import "ConversionStdString.h"

#include "MachO/machoexception.h"
#include "MachO/symboltablecommand.h"

#import "SymbolTableEntryModel.h"
#import "MyDocument.h"

// declare private methods
@interface ArchitectureModel ()
- (void) initSymbols;
@end


@implementation ArchitectureModel

- (id) initWithArchitecture:(MachOArchitecture*)anArchitecture file:(MachO*)aFile document:(MyDocument*)aDocument isRoot:(BOOL)isRoot{
	architecture = anArchitecture;
	file = aFile;
	document = aDocument;
	symbolEntries = nil;
	
	// create model with the current architecture selected (create from existing model)
	if (isRoot) {
		machOModel = nil;
	} else {
		machOModel = [[MachOModel alloc]initWithFile:aFile document:aDocument architecture:anArchitecture loadChildren:NO];
	}
	
	return self;
}

- (void)dealloc {
	[symbolEntries release];
	[machOModel release];
	[super dealloc];
}

- (NSArray*) rootModel {
	// always reload model (for error messages)
	[machOModel release];
	machOModel = [[MachOModel alloc]initWithFile:file document:document architecture:architecture loadChildren:YES];
	NSArray* rootModel = [NSArray arrayWithObject:machOModel];
	return rootModel;
}

+ (NSString*) cpuType:(MachOHeader::CpuType)cpuType {
	NSString* label;
	switch (cpuType) {
		case MachOHeader::CpuTypePowerPc:
			label = NSLocalizedString(@"CPU_TYPE_PPC", nil);
			break;
		case MachOHeader::CpuTypeI386:
			label = NSLocalizedString(@"CPU_TYPE_I386", nil);
			break;
		case MachOHeader::CpuTypePowerPc64:
			label = NSLocalizedString(@"CPU_TYPE_PPC64", nil);
			break;
		case MachOHeader::CpuTypeX8664:
			label = NSLocalizedString(@"CPU_TYPE_X8664", nil);
			break;
		default:
			label = NSLocalizedString(@"UNDEFINED", nil);
			break;
			
	}
	return label;
}

- (NSString*) label {
	return [ArchitectureModel cpuType:architecture->getHeader()->getCpuType()];
}

- (NSString*) uuid {
	// use CFUUID
	CFUUIDRef uuid = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault, *((CFUUIDBytes*)architecture->getUuid()));
	CFStringRef result = CFUUIDCreateString(kCFAllocatorDefault, uuid);
	CFRelease(uuid);
	return (NSString*) result;
}

- (NSString*) fileType {
	NSString* type;
	switch(architecture->getHeader()->getFileType())  {
		case MachOHeader::FileTypeObject:		/* relocatable object file */
			type = NSLocalizedString(@"FILE_TYPE_OBJECT", nil);
			break;
		case MachOHeader::FileTypeExecutable:    		/* demand paged executable file */
			type = NSLocalizedString(@"FILE_TYPE_EXECUTABLE", nil);
			break;
		case MachOHeader::FileTypeVmLib:  		/* fixed VM shared library file */
			type = NSLocalizedString(@"FILE_TYPE_VM_SHARED_LIBRARY", nil);
			break;
		case MachOHeader::FileTypeCore:   		/* core file */
			type = NSLocalizedString(@"FILE_TYPE_CORE", nil);
			break;
		case MachOHeader::FileTypePreload:                /* preloaded executable file */
			type = NSLocalizedString(@"FILE_TYPE_PRELOADED_EXECUTABLE", nil);
			break;
		case MachOHeader::FileTypeDylib:          	/* dynamically bound shared library */
			type = NSLocalizedString(@"FILE_TYPE_SHARED_LIBRARY", nil);
			break;
		case MachOHeader::FileTypeDylinker:               /* dynamic link editor */
			type = NSLocalizedString(@"FILE_TYPE_DYNAMIC_LINKER", nil);
			break;
		case MachOHeader::FileTypeBundle:                 /* dynamically bound bundle file */
			type = NSLocalizedString(@"FILE_TYPE_BUNDLE", nil);
			break;
		case MachOHeader::FileTypeDylibStub:              /* shared library stub for static linking only, no section contents */
			type = NSLocalizedString(@"FILE_TYPE_STATIC_LIBRARY", nil);
			break;
		case MachOHeader::FileTypeDsym:   		/* companion file with only debug sections */
			type = NSLocalizedString(@"FILE_TYPE_DSYM", nil);
			break;
		case MachOHeader::FileTypeKextBundle:  		/* kext bundle */
			type = NSLocalizedString(@"FILE_TYPE_KEXT_BUNDLE", nil);
			break;
		default:
			type = NSLocalizedString(@"UNDEFINED", @"Unknown");
	}
	return type;
}

- (NSString*) identifier {
	NSString* identifier = [NSString string];
	DylibCommand* dynamicLibraryIdCommand = architecture->getDynamicLibIdCommand();
	if (dynamicLibraryIdCommand != NULL) {
		identifier = [NSString stringWithStdString:dynamicLibraryIdCommand->getName()];
	}
	return identifier;
}

- (NSString*) dynamicLinker {
	NSString* dynamicLinker;
	dynamicLinker = [NSString stringWithStdString:architecture->getDynamicLinker()];
	return dynamicLinker;
}

- (NSString*) rpath {
	std::vector<string*> rpaths = architecture->getRpaths(false);
	NSMutableString* rpath = [NSMutableString string];
	for (std::vector<string*>::iterator it = rpaths.begin();
		 it != rpaths.end();
		 ++it)
    {
        [rpath appendFormat:@"%@; ", [NSString stringWithStdString:**it]];
    }
	return rpath;
}

- (NSNumber*) showIdentifier {
	NSNumber* result;
	if ([[self identifier] length] == 0) {
		result = [NSNumber numberWithBool:true];
	} else {
		result = [NSNumber numberWithBool:false];
	}
	return result;
}

- (NSString*) size {
	NSString* size;
	// regard the correct format specifier %qu for unsigned long
	unsigned int architectureSize = architecture->getSize();
	unsigned long long fileSize = file->getSize();
	if (fileSize != architectureSize) {
		size = [NSString stringWithFormat:NSLocalizedString(@"SIZE_FORMAT_ARCHITECTURE", nil), 
				fileSize/1024L, architectureSize/1024];
	} else {
		size = [NSString stringWithFormat:NSLocalizedString(@"SIZE_FORMAT", nil), 
				fileSize/1024L];
	}
	return size;
}

- (NSString*) version {
	NSString* version = @"?";
	DylibCommand* dynamicLibraryIdCommand = architecture->getDynamicLibIdCommand();
	if (dynamicLibraryIdCommand != NULL) {
		VersionFormatter* versionFormatter = [[VersionFormatter alloc] init];
		
		time_t timestamp = dynamicLibraryIdCommand->getTimeStamp();
		NSString* currentVersion = [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:dynamicLibraryIdCommand->getCurrentVersion()]];
		NSString* compatibleVersion = [versionFormatter stringForObjectValue:[NSNumber numberWithUnsignedInt:dynamicLibraryIdCommand->getCompatibleVersion()]];
		if ([currentVersion length]+[compatibleVersion length] > 0) {
			if (timestamp > 1) {
				NSDate* date = [NSDate dateWithTimeIntervalSince1970:timestamp];
				
				// this date formatter should be identical to NSDateFormatter in IB
				NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
				[dateFormatter setDateStyle:NSDateFormatterMediumStyle];
				[dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
				
				version = [NSString stringWithFormat:NSLocalizedString(@"VERSION_FORMAT_TIMESTAMP", nil), 
						   currentVersion,
						   compatibleVersion,
						   [dateFormatter stringFromDate:date]];
			}
			else {
				version = [NSString stringWithFormat:NSLocalizedString(@"VERSION_FORMAT", nil), 
						   currentVersion,
						   compatibleVersion];
			}
		}
		[versionFormatter release];
	} else {
		version = [machOModel version];
	}
	return version;
}


- (void) initSymbols {
	symbolEntries = [NSMutableArray arrayWithCapacity:20];
	[symbolEntries retain];
	for (MachOArchitecture::LoadCommandsConstIterator lcIter = architecture->getLoadCommandsBegin();
		 lcIter != architecture->getLoadCommandsEnd();
		 ++lcIter)
	{
		// check if it is dylibcommand
		SymbolTableCommand* command = dynamic_cast<SymbolTableCommand*> (*lcIter);
		if (command != 0) {
			for (SymbolTableCommand::SymbolTableEntriesConstIterator it = command->getSymbolTableEntryBegin(); it != command->getSymbolTableEntryEnd(); it++) {
				SymbolTableEntryModel* symbolModel = [[SymbolTableEntryModel alloc] initWithEntry:*(it) demangleNamesPtr:[[document symbolTableController]demangleNamesPtr] document:document];
				[symbolEntries addObject:symbolModel];
			}
		}
	}
}

- (NSArray*) symbols {
	if (symbolEntries == nil) {
		[self initSymbols];
	}
	return symbolEntries;
}

- (void) setSymbols:(NSMutableArray*) newSymbolEntries {
	[newSymbolEntries retain];
	[symbolEntries release];
	symbolEntries = symbolEntries;
}

- (void) refreshSymbols {
	[symbolEntries release];
	[self initSymbols];
	[self setSymbols:symbolEntries];
}



@end
