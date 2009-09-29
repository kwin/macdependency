//
//  ConversionStdString.m
//  MacDependency
//
//  Created by Konrad Windszus on 17.07.09.
//  Copyright 2009 Konrad Windszus. All rights reserved.
//
//  Conversion functions for NSString from/to std::string and std::wstring
#import "ConversionStdString.h"


@implementation NSString (ConversionStdString)
+ (NSString*) stringWithStdString:(const std::string&)string {
	return [NSString stringWithCString:string.c_str() encoding:NSASCIIStringEncoding];
}

- (std::string) stdString {
	return std::string([self cStringUsingEncoding:NSASCIIStringEncoding]);
}

#if TARGET_RT_BIG_ENDIAN
const NSStringEncoding kEncoding_wchar_t = 
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32BE);
#else
const NSStringEncoding kEncoding_wchar_t = 
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
#endif

+(NSString*) stringWithStdWString:(const std::wstring&)ws
{
	char* data = (char*)ws.data();
	size_t size = ws.size() * sizeof(wchar_t);
	
	NSString* result = [[[NSString alloc] initWithBytes:data length:size 
											   encoding:kEncoding_wchar_t] autorelease];
	return result;
}

-(std::wstring) stdWString
{
	NSData* asData = [self dataUsingEncoding:kEncoding_wchar_t];
	return std::wstring((wchar_t*)[asData bytes], [asData length] / 
						sizeof(wchar_t));
}

@end
