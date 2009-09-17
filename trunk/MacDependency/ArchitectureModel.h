//
//  ArchitectureModel.h
//  MacDependency
//
//  Created by Konrad Windszus on 22.08.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MachOModel.h"
#include "macho/machoarchitecture.h"
#include "MachO/machoheader.h"

@class MyDocument;

@interface ArchitectureModel : NSObject {
	MachOArchitecture* architecture;
	MachOModel* machOModel;
	MachO* file;
	MyDocument* document;
	NSMutableArray* symbolEntries;
	
}
- (id) initWithArchitecture:(MachOArchitecture*)architecture file:(MachO*)file document:(MyDocument*)document isRoot:(BOOL)isRoot;
- (NSString*) label;
- (NSString*) fileType;
- (NSString*) idName;
- (NSString*) size;
- (NSString*) version;

+ (NSString*) cpuType:(MachOHeader::CpuType)cpuType;

- (void) refreshSymbols;
- (NSArray*) symbols;
- (void) setSymbols:(NSMutableArray*) symbolEntries;
@end
