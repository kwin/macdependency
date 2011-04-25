//
//  MyDocument.m
//  MacDependency
//
//  Created by Konrad Windszus on 19.06.09.
//  Copyright Konrad Windszus 2009 . All rights reserved.
//

#import "MyDocument.h"
#import "ConversionStdString.h"
#import "MachOModel.h"
#import "TreeControllerExtension.h"
#import "ArchitectureModel.h"
#include "MachO/machoexception.h"



// declare private methods
@interface MyDocument ()
- (NSString*) serializeIndexPath:(NSIndexPath*)indexPath;
- (NSIndexPath*) deserializeIndexPath:(NSString*)link;
@end

@implementation MyDocument

- (id)init
{
    self = [super init];
    if (self) {
		
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.
		//contents = [[NSMutableArray alloc] init];
		cache = new MachOCache();
		log = [[NSAttributedString alloc] initWithString:@""];
		numDependencies = 0;
		splitViewDelegate = [[PrioritySplitViewDelegate alloc] init];
		
    }
    return self;
}

- (void) dealloc
{
	delete cache;
	[splitViewDelegate release];
	[contents release];
	[log release];
	[super dealloc];
}


- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
	
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}


- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
    // Insert code here to write your document to data of the specified type. If the given outError != NULL, ensure that you set *outError when returning nil.
	
    // You can also choose to override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.
	
    // For applications targeted for Panther or earlier systems, you should use the deprecated API -dataRepresentationOfType:. In this case you can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.
	
    if ( outError != NULL ) {
		*outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
	}
	return nil;
}


- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{	
	// TODO: detect changes on document file (FSEvents) or alternatively make reload possible
	
	// load file
	NSString* filePath = [absoluteURL path];
	
	// convert to std:string
	std::string fileString = [filePath stdString];
	try {
		machO = cache->getFile(fileString, NULL);
	} catch (MachOException& exc) {
		NSString* msg = [NSString stringWithUTF8String:exc.getCause().c_str()];
		
		
		// create and return custom domain error (the localized description key is overwritten by OS, so no point in setting it here)
		NSArray *objArray = [NSArray arrayWithObjects:@"", msg, @"Try again with another file", nil];
		NSArray *keyArray = [NSArray arrayWithObjects:NSLocalizedDescriptionKey, NSLocalizedFailureReasonErrorKey, NSLocalizedRecoverySuggestionErrorKey, nil];
		
		NSDictionary *eDict = [NSDictionary dictionaryWithObjects:objArray forKeys:keyArray];
		
		// fill outError
		*outError = [NSError errorWithDomain:@"MachO" code:0 userInfo:eDict];
		return NO;
	}
	return YES;
}

-(void)awakeFromNib {
	[[textFieldBottomBar cell] setBackgroundStyle:NSBackgroundStyleRaised]; 
	
	[splitViewDelegate setPriority:1 forViewAtIndex:0];
	[splitViewDelegate setPriority:0 forViewAtIndex:1];
	[splitViewDelegate setMinimumLength:150 forViewAtIndex:0];
	[splitViewDelegate setMinimumLength:400 forViewAtIndex:1];
	[mainSplitView setDelegate:splitViewDelegate];
}

- (NSAttributedString*)log {
	return log;
}

- (void)setLog:(NSAttributedString *)newLog {
	[newLog retain];
	[log release];
	log = newLog;
}

- (void)clearLog {
	NSAttributedString* newLog = [[NSAttributedString alloc] initWithString:@""];
	[self setLog:newLog];
	[newLog release];
}

- (void)appendLogLine:(NSString *)line withModel:(MachOModel*)model state:(State)state {
	
	NSMutableAttributedString* newLog = [[NSMutableAttributedString alloc] init];
	[newLog appendAttributedString:log];
	
	NSString* prefix;
	switch (state) {
		case StateError:
			prefix = NSLocalizedString(@"LOG_PREFIX_ERROR", nil);
			break;
		default:
			prefix = NSLocalizedString(@"LOG_PREFIX_WARNING", nil);
			break;
	}
	
	NSString* newLine = [NSString stringWithFormat:@"%@%@\n\n", prefix, line];
	NSDictionary* attributes;
	
	if (model) {
		attributes = [NSDictionary dictionaryWithObjectsAndKeys:model, NSLinkAttributeName, [NSCursor pointingHandCursor], NSCursorAttributeName, NSLocalizedString(@"LOG_LINK_TOOLTIP", nil), NSToolTipAttributeName, nil];
	} else {
		attributes = [NSDictionary dictionary];
	}
	NSAttributedString* newLogLine = [[NSAttributedString alloc]initWithString:newLine attributes:attributes];
	
	[newLog appendAttributedString:newLogLine];
	[newLogLine release];
	[self setLog:newLog];
	[newLog release];
}

- (NSString*) workingDirectory {
	// don't release the returned string!!, apparently then filename is released also
    NSString* filePath = [[super fileURL] path];
	return [filePath stringByDeletingLastPathComponent];
}


- (NSString*) serializeIndexPath:(NSIndexPath*)indexPath {
	NSMutableString* link = [NSMutableString stringWithCapacity:20];
	for (int depth = 0; depth < [indexPath length]; depth++) {
		[link appendFormat:@"%d;", [indexPath indexAtPosition:depth]];
	}
	return link;
}

- (NSIndexPath*) deserializeIndexPath:(NSString*)link {
	NSIndexPath* indexPath;
	
	// tokenize string
	NSArray* indices = [link componentsSeparatedByString:@";"];
	
	// go through tokens
	NSEnumerator *enumerator = [indices objectEnumerator];
	NSString* token = [enumerator nextObject];
	if (token) {
		indexPath = [NSIndexPath indexPathWithIndex:[token intValue]];
		while ((token = [enumerator nextObject])) {
			if ([token length] > 0)
				indexPath = [indexPath indexPathByAddingIndex:[token intValue]];
		}
	}
	return indexPath;
}


// delegate method 
- (BOOL)textView:(NSTextView *)aTextView clickedOnLink:(id)link atIndex:(NSUInteger)charIndex {
	[dependenciesController setSelectedObject:link];
	
	// we need no further processing of the link
	return YES;
}


- (MachOCache*)cache {
	return cache;
}

- (NSArray*) architectures {
	NSMutableArray* architectures = [NSMutableArray arrayWithCapacity:4];
	
	if (machO) {
		MachOArchitecture* architecture = machO->getHostCompatibleArchitecture();
		if (!architecture) {
			// no host-compatible architecture found (just take first architecture)
			architecture = *(machO->getArchitecturesBegin());
		}
		for (MachO::MachOArchitecturesIterator iter = machO->getArchitecturesBegin(); iter != machO->getArchitecturesEnd(); iter++) {
			// create model for architecture
			ArchitectureModel* currentArchitecture = [[ArchitectureModel alloc]initWithArchitecture:(*iter) file:machO document:self isRoot:YES];
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

- (IBAction)clickRevealInFinder:(id)sender {
	NSString* filename = [textFieldFilename stringValue];
	
	[[NSWorkspace sharedWorkspace] selectFile:filename 
					 inFileViewerRootedAtPath:nil];

}

- (void)incrementNumDependencies {
	[self willChangeValueForKey:@"dependencyStatus"];
	numDependencies++;
	[self didChangeValueForKey:@"dependencyStatus"];
}

- (void)resetNumDependencies {
	[self willChangeValueForKey:@"dependencyStatus"];
	numDependencies = 0;
	[self didChangeValueForKey:@"dependencyStatus"];
}


- (NSString*)dependencyStatus {
	NSString* status = [NSString stringWithFormat:NSLocalizedString(@"DEPENDENCY_STATUS", nil), numDependencies, cache->getNumEntries()];
	return status;
}

- (SymbolTableController*)symbolTableController {
	return symbolTableController;
}

@end
