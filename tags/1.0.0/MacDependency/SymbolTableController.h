//
//  SymbolTableController.h
//  MacDependency
//
//  Created by Konrad Windszus on 18.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class MyDocument;

@interface SymbolTableController : NSArrayController {
	NSPredicate* nameFilter;
	IBOutlet NSSegmentedControl* typeFilterControl;
	IBOutlet NSButton* demangleNamesControl;
	BOOL demangleNames;
	IBOutlet MyDocument* document;
}

- (void)setNameFilter:(NSPredicate*) nameFilter;
- (NSPredicate*)nameFilter;

- (IBAction)typeFilterChanged:(id)sender;

- (BOOL)demangleNames;
- (BOOL*)demangleNamesPtr;
- (void)setDemangleNames:(BOOL)demangleNames;

@end
