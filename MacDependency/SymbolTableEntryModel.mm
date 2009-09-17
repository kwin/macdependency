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

@implementation SymbolTableEntryModel

- (id) initWithEntry:(const SymbolTableEntry*)entry demangleNamesPtr:(BOOL*)demangleNames  {
	if (self = [super init]) {
		self->entry = entry;
		self->demangleNames = demangleNames;
	}
	return self;
}

- (NSString*) name {
	//try {
		return [NSString stringWithStdString:entry->getName(*demangleNames)];
	/*} catch (MachODemangleException& e) {
		
		// TODO: disable demangling and show error message
		NSLog([NSString stringWithStdString:e.getCause()]);
		return [NSString stringWithStdString:entry->getName(false)];

	}*/
}

- (NSNumber*) type {
	return [NSNumber numberWithUnsignedInt:entry->getType()];
}

@end
