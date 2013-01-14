//
//  DebugOutput.m
//  PictureMe
//
//  Created by Ashish Lal on 25/02/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "DebugOutput.h"


@implementation DebugOutput
static DebugOutput *sharedDebugInstance = nil;

/*---------------------------------------------------------------------*/
+ (DebugOutput *) sharedDebug
{
	@synchronized(self)
	{
		if (sharedDebugInstance == nil)
		{
			[[self alloc] init];
		}
	}
	return sharedDebugInstance;
}

/*---------------------------------------------------------------------*/
+ (id) allocWithZone:(NSZone *) zone
{
	@synchronized(self)
	{
		if (sharedDebugInstance == nil)
		{
			sharedDebugInstance = [super allocWithZone:zone];
			return sharedDebugInstance;
		}
	}
	return nil;
}

/*---------------------------------------------------------------------*/
- (id)copyWithZone:(NSZone *)zone
{
	return self;
}

/*---------------------------------------------------------------------*/
- (id)retain
{
	return self;
}

/*---------------------------------------------------------------------*/
- (void)release
{
	// No action required...
}

/*---------------------------------------------------------------------*/
- (unsigned)retainCount
{
	return UINT_MAX;  // An object that cannot be released
}

/*---------------------------------------------------------------------*/
- (id)autorelease
{
	return self;
}

/*---------------------------------------------------------------------*/
-(void)output:(char*)fileName lineNumber:(int)lineNumber input:(NSString*)input, ...
{
	va_list argList;
	NSString *filePath, *formatStr;
	
	// Build the path string
	filePath = [[NSString alloc] initWithBytes:fileName length:strlen(fileName)
									  encoding:NSUTF8StringEncoding];
	
	// Process arguments, resulting in a format string
	va_start(argList, input);
	formatStr = [[NSString alloc] initWithFormat:input arguments:argList];
	va_end(argList);
	
	// Call NSLog, prepending the filename and line number
	NSLog(@"File:%s Line:%d %@",[((DEBUG_SHOW_FULLPATH) ? filePath :
								  [filePath lastPathComponent]) UTF8String], lineNumber, formatStr);
	
	[filePath release];
	[formatStr release];
}

@end
