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

- (id) initWithEntry:(const SymbolTableEntry*)anEntry demangleNamesPtr:(BOOL*)demangleNames document:(MyDocument*)aDocument {
	self = [super init];
    if (self) {
		entry = anEntry;
		self->demangleNames = demangleNames;
		document = aDocument;
	}
	return self;
}

- (NSString*) name {
	try {
		return [NSString stringWithStdString:entry->getName(*demangleNames)];
	} catch (MachODemangleException& e) {
        // in case of demangling problems (probably c++filt not installed)
		NSString* error = NSLocalizedString(@"ERR_NO_DEMANGLER", nil);
		[document appendLogLine:error withModel:nil state:StateError];
        // disable name demangling
		[[document symbolTableController] setDemangleNames:NO];
	}
	return [NSString stringWithStdString:entry->getName(false)];
}

- (NSNumber*) type {
	return [NSNumber numberWithUnsignedInt:entry->getType()];
}

@end
