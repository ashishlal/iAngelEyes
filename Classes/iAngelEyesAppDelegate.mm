#import "iAngelEyesAppDelegate.h"
#import <UIKit/UIKit.h>
//#import <UIKit/UIProgressBar.h>

@implementation iAngelEyesAppDelegate


@synthesize window;
@synthesize main;


- (void)applicationDidFinishLaunching:(UIApplication *)application {  
	[window addSubview:[main view]];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [window release];
    [super dealloc];
}


@end
