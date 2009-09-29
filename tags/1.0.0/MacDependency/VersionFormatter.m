//
//  VersionFormatter.m
//  MacDependency
//
//  Created by Konrad Windszus on 17.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Formatter for version integers  (4 byte) in the form xxxx.xx.xx
#import "VersionFormatter.h"

#define HIBYTE(x)	( (unsigned char) ((x) >> 8) )
#define LOBYTE(x)	( (unsigned char) (x) )
#define HIWORD(x)	( (unsigned short) ( (x) >> 16) )
#define LOWORD(x)	( (unsigned short) (x) )

@implementation VersionFormatter

// conversion to string
- (NSString*) stringForObjectValue:(id)obj {
	// must be a NSNumber
	if (![obj isKindOfClass:[NSNumber class]]) {
		return nil;
	}
	
	// NSNumber contains the version as unsigned int
	unsigned int version = [obj unsignedIntValue];
	
	if (version == 0) {
		return [NSString string];
	}
	NSString* versionString = [NSString stringWithFormat:@"%d.%d.%d", HIWORD(version), (unsigned short)HIBYTE(LOWORD(version)), (unsigned short)LOBYTE(LOWORD(version)) ];
	return versionString;
}


// conversion from string (not necessary)
- (BOOL) getObjectValue:(id*)obj forString:(NSString*)string errorDescription:(NSString**)errorString {
	return NO;
}
@end
