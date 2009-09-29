//
//  SymbolEntryModel.h
//  MacDependency
//
//  Created by Konrad Windszus on 13.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "MachO/machofile.h"
#include "MachO/symboltableentry.h"
@class MyDocument;

@interface SymbolTableEntryModel : NSObject {
	const SymbolTableEntry* entry;
	const BOOL* demangleNames;
	MyDocument* document;
}

- (id) initWithEntry:(const SymbolTableEntry*)entry demangleNamesPtr:(BOOL*)demangleNames document:(MyDocument*)document;
- (NSString*) name;
- (NSNumber*) type;
@end
