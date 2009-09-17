//
//  AutoExpandOutlineView.m
//  MacDependency
//
//  Created by Konrad Windszus on 03.09.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import "AutoExpandOutlineView.h"


@implementation AutoExpandOutlineView


// gets called after binding has changed
- (void)reloadData;
{
	[super reloadData];

	// auto expand root item
	NSTreeNode* item = [self itemAtRow:0];
	if (item)
		[self expandItem:item];
}

@end
