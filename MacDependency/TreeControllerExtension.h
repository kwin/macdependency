//
//  TreeControllerExtension.h
//  MacDependency
//
//  Created by Konrad Windszus on 02.09.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
// NSTreeController-DMExtensions.h
// Library
//
// Created by William Shipley on 3/10/06.
// Copyright 2006 Delicious Monster Software, LLC. Some rights reserved,
// see Creative Commons license on wilshipley.com
// @see http://wilshipley.com/blog/2006/04/pimp-my-code-part-10-whining-about.html
#import <Cocoa/Cocoa.h>
#import "ExtTreeModel.h"

@interface NSTreeController (TreeControllerExtension)
- (BOOL)setSelectedObjects:(NSArray *)newSelectedObjects;
- (BOOL)setSelectedObject:(id <ExtTreeModel>)object;
- (NSIndexPath *)indexPathToObject:(id <ExtTreeModel>)object;
@end
