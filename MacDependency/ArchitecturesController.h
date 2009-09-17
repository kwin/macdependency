//
//  ArchitecturesController.h
//  MacDependency
//
//  Created by Konrad Windszus on 22.08.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class MyDocument;

@interface ArchitecturesController : NSArrayController {
	IBOutlet MyDocument* document;
}
@end
