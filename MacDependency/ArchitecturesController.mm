//
//  ArchitecturesController.m
//  MacDependency
//
//  Created by Konrad Windszus on 22.08.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Only used for root architectures.
//
#import "ArchitecturesController.h"
#import "MyDocument.h"

// declare private methods
@interface ArchitecturesController ()
@end

@implementation ArchitecturesController

- (id)initWithCoder:(NSCoder *)decoder {
	self = [super initWithCoder:decoder];
    if (self) {
		
	}
    return self;
}

- (void)dealloc
{
	[super dealloc];
}


- (void)awakeFromNib
{
	[super awakeFromNib];
}

- (BOOL)setSelectionIndex:(NSUInteger)index {
	// clear log
	[document clearLog];
	[document resetNumDependencies];
	return [super setSelectionIndex:index];
}



@end
