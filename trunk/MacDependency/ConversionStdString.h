//
//  ConversionStdString.h
//  MacDependency
//
//  Created by Konrad Windszus on 17.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <string>

@interface NSString (ConversionStdString) 

+ (NSString*) stringWithStdString:(const std::string&)string;
- (std::string) stdString;
+ (NSString*) stringWithStdWString:(const std::wstring&)string;
- (std::wstring) stdWString;
@end
