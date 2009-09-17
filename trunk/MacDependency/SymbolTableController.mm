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
#include "MachO/machodemangleexception.h"

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


- (void)setNameFilter:(NSPredicate*) nameFilter {
	[nameFilter retain];
	[self->nameFilter release];
	self->nameFilter = nameFilter;
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
	
	/*// TODO: call refreshSymbols for the current architecture (over ArchitecturesController)
	NSArray* objects = [dependenciesController selectedObjects];
	if (objects && [objects lastObject]) {
		MachOModel* model = [objects lastObject];
		//[model refreshSymbols];
	}*/
}

- (id)valueForKeyPath:(NSString *)keyPath {
	try {
		[super valueForKeyPath:keyPath];
	}
	catch (MachODemangleException& exc) {
		NSString* error = NSLocalizedString(@"ERR_NO_DEMANGLER", nil);
		[document appendLogLine:error withModel:nil state:StateError];
	}
}

@end
