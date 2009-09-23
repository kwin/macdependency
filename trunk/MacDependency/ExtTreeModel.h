//
//  ExtTreeModel.h
//  MacDependency
//
//  Created by Konrad Windszus on 02.09.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Protocol which provides a method to get the parent (used by TreeControllerExtension)
#import <Cocoa/Cocoa.h>


@protocol ExtTreeModel<NSObject>
- (id) parent;
@end
