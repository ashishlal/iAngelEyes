//
//  iPictureMeAppDelegate.h
//  iPictureMe
//
//  Created by Ashish Lal on 21/03/10.
//  Copyright NetTech India 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PictureMeController;

@interface iPictureMeAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    PictureMeController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PictureMeController *viewController;

@end

