//
//  SymbolTableController.m
//  MacDependency
//
//  Created by Konrad Windszus on 18.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import "SymbolTableController.h"
#import "MyDocument.h"
#include "MachO/symboltableentry.h"


@interface SymbolTableController()
-(void)setFilter;
- (NSPredicate*) typeFilter;
@end

@implementation SymbolTableController


const int TYPE[] = {SymbolTableEntry::TypeExported, SymbolTableEntry::TypeImported};

- (id)initWithCoder:(NSCoder *)decoder {
	self = [super initWithCoder:decoder];
    if (self) {
		demangleNames = true;
	}
    return self;
}

// called when all connections were made
- (void)awakeFromNib {
	[self setFilter];
}

- (NSPredicate*) typeFilter {
	NSMutableString* typeFilter = [NSMutableString string];
	
	for (int segment=0; segment < sizeof(TYPE)/sizeof(*TYPE); segment++) {
		if ([typeFilterControl isSelectedForSegment:segment]) {
			NSString* condition = [NSString stringWithFormat:@"type=%d", TYPE[segment]];
			if ([typeFilter length] > 0) {
				[typeFilter appendString:@" or "];
			} 
			[typeFilter appendString:condition];
		}
	}
	
	// select nothing if no filter set
	NSPredicate* predicate;
	if ([typeFilter length] == 0) {
		predicate = [NSPredicate predicateWithValue:NO];
	} else {
		predicate = [NSPredicate predicateWithFormat:typeFilter];
	}
	return predicate;
}

- (NSPredicate*)nameFilter {
	return nameFilter;
}


- (void)setNameFilter:(NSPredicate*) newNameFilter {
	[newNameFilter retain];
	[nameFilter release];
	nameFilter = newNameFilter;
	[self setFilter];
}

- (IBAction)typeFilterChanged:(id)sender {
	[self setFilter];
}

-(void)setFilter {
	NSPredicate* typeFilter = [self typeFilter];
	NSPredicate* predicate;
	if (nameFilter) {
		predicate = [NSCompoundPredicate andPredicateWithSubpredicates:[NSArray arrayWithObjects:nameFilter, typeFilter, nil]]; 
	} else {
		predicate = typeFilter;
	}
	//NSLog(@"%@", predicate);
	[self setFilterPredicate:predicate];
}

- (BOOL)demangleNames {
	return demangleNames;
}

- (BOOL*)demangleNamesPtr {
	return &demangleNames;
}

- (void)setDemangleNames:(BOOL)demangleNames {
	self->demangleNames = demangleNames;
	
	// refresh
	[self rearrangeObjects];
}

@end
