//
//  TouchCameraAppDelegate.h
//  TouchCamera
//
#import <UIKit/UIKit.h>
#import "iAngelEyesController.h"

@interface iAngelEyesAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    iAngelEyesController *main;
	
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet iAngelEyesController *main;

@end

