//
//  SymbolTableEntryTypeFormatter.m
//  MacDependency
//
//  Created by Konrad Windszus on 18.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Formatter for types of symbol table entries. We need a formatter to provide a valid sort order.
#import "SymbolTableEntryTypeFormatter.h"
#include "MachO/symboltableentry.h"

@implementation SymbolTableEntryTypeFormatter
// conversion to string
- (NSString*) stringForObjectValue:(id)obj {
	// must be a NSNumber
	if (![obj isKindOfClass:[NSNumber class]]) {
		return nil;
	}
	
	// NSNumber contains the version as unsigned int
	unsigned int typeNumber = [obj unsignedIntValue];
	NSString* type;
	switch(typeNumber) {
		case SymbolTableEntry::TypeExported:
			type = NSLocalizedString(@"SYMBOL_TYPE_EXPORT", @"Export");
			break;
		case SymbolTableEntry::TypeImported:
			type = NSLocalizedString(@"SYMBOL_TYPE_IMPORT", @"Import");
			break;
		default:
			type = NSLocalizedString(@"UNKNOWN", @"Unknown");
			
	}
	return type;
}


// conversion from string (not necessary)
- (BOOL) getObjectValue:(id*)obj forString:(NSString*)string errorDescription:(NSString**)errorString {
	return NO;
}
@end
