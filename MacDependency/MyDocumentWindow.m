//
//  MyDocumentWindow.m
//  MacDependency
//
//  Created by Konrad Windszus on 04.09.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Window class for document windows
//
#import "MyDocumentWindow.h"


@implementation MyDocumentWindow

-(void)awakeFromNib {
	[super awakeFromNib];
	
	// set bottom bar
	[self setContentBorderThickness:24.0 forEdge:NSMinYEdge];
}
@end
