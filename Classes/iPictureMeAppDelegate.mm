//
//  iPictureMeAppDelegate.m
//  iPictureMe
//
//  Created by Ashish Lal on 21/03/10.
//  Copyright NetTech India 2010. All rights reserved.
//

#import "iPictureMeAppDelegate.h"
#import "iPictureMeViewController.h"

@implementation iPictureMeAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
