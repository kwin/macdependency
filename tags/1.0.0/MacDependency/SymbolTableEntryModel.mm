//
//  SymbolEntryModel.mm
//  MacDependency
//
//  Created by Konrad Windszus on 13.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import "SymbolTableEntryModel.h"
#import "MachOModel.h"
#import "ConversionStdString.h"
#import "MyDocument.h"
#include "MachO/machodemangleexception.h"

@implementation SymbolTableEntryModel

- (id) initWithEntry:(const SymbolTableEntry*)entry demangleNamesPtr:(BOOL*)demangleNames document:(MyDocument*)document {
	if (self = [super init]) {
		self->entry = entry;
		self->demangleNames = demangleNames;
		self->document = document;
	}
	return self;
}

- (NSString*) name {
	try {
		return [NSString stringWithStdString:entry->getName(*demangleNames)];
	} catch (MachODemangleException& e) {
		NSString* error = NSLocalizedString(@"ERR_NO_DEMANGLER", nil);
		[document appendLogLine:error withModel:nil state:StateError];
		[[document symbolTableController] setDemangleNames:NO];
	}
	return [NSString stringWithStdString:entry->getName(false)];
}

- (NSNumber*) type {
	return [NSNumber numberWithUnsignedInt:entry->getType()];
}

@end
