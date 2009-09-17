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

@interface SymbolTableEntryModel : NSObject {
	const SymbolTableEntry* entry;
	const BOOL* demangleNames;
}

- (id) initWithEntry:(const SymbolTableEntry*) entry demangleNamesPtr:(BOOL*)demangleNames;
- (NSString*) name;
- (NSNumber*) type;
@end
