//
//  TreeControllerExtension.m
//  MacDependency
//
//  Created by Konrad Windszus on 02.09.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Extension to NSTreeController to provide methods to select an item if only its data pointer is known.


#import "TreeControllerExtension.h"


@interface NSTreeController (TreeControllerExtension_Private)
- (NSIndexPath *)_indexPathFromIndexPath:(NSIndexPath *)baseIndexPath inChildren:(NSArray **)children
								toObject:(id <ExtTreeModel>)object;
@end


@implementation NSTreeController (TreeControllerExtension)

- (BOOL)setSelectedObjects:(NSArray *)newSelectedObjects
{
	NSMutableArray *indexPaths = [NSMutableArray array];
	unsigned int selectedObjectIndex;
	for (selectedObjectIndex = 0; selectedObjectIndex < [newSelectedObjects count];
		 selectedObjectIndex++) {
		id selectedObject = [newSelectedObjects objectAtIndex:selectedObjectIndex];
		NSIndexPath *indexPath = [self indexPathToObject:selectedObject];
		if (indexPath)
			[indexPaths addObject:indexPath];
	}
	return [self setSelectionIndexPaths:indexPaths];
}

- (BOOL)setSelectedObject:(id <ExtTreeModel>)object
{
	NSIndexPath *indexPath = [self indexPathToObject:object];
	if (indexPath) {
		return [self setSelectionIndexPath:indexPath];
	}
	return NO;
}

- (NSIndexPath *)indexPathToObject:(id <ExtTreeModel>)object
{
	NSArray *children = [self content];
	return [self _indexPathFromIndexPath:nil inChildren:&children toObject:object];
}

@end

@implementation NSTreeController (TreeControllerExtension_Private)

// inspired by: http://wilshipley.com/blog/2006/04/pimp-my-code-part-10-whining-about.html but much faster and does not rely
// on dirty and undocumented selectors of arrangedObjects.
- (NSIndexPath *)_indexPathFromIndexPath:(NSIndexPath *)baseIndexPath inChildren:(NSArray **)children
							  toObject:(id <ExtTreeModel>)object;
{
	
	NSIndexPath* indexPath = nil;
	// go back to root
	id <ExtTreeModel> parent = [object parent];
	if (parent) {
		baseIndexPath = [self _indexPathFromIndexPath:baseIndexPath inChildren:children toObject:parent];
		if (!baseIndexPath)
			return nil;
	} 
	
	// go through children (content)
	unsigned int childIndex;
	NSMutableArray* sortedChildren = [NSMutableArray arrayWithArray:*children];
	[sortedChildren sortUsingDescriptors:[self sortDescriptors]];

	for (childIndex = 0; childIndex < [sortedChildren count]; childIndex++) {
		id childObject = [sortedChildren objectAtIndex:childIndex];
		if ([object isEqual:childObject]) {
			
			NSString *childrenKeyPath = [self childrenKeyPath];
			NSArray* childsChildren = [childObject valueForKey:childrenKeyPath];
			*children = childsChildren;
			
			// create current NSIndex
			if (!baseIndexPath) {
				indexPath = [NSIndexPath indexPathWithIndex:childIndex];
			} else {
				 indexPath = [baseIndexPath indexPathByAddingIndex:childIndex];
			}
			return indexPath;
		}
	}
	return nil; // something wrong here (can't find object)
}

@end