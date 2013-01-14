//
//  iPictureMeViewController.h
//  iPictureMe
//
//  Created by Ashish Lal on 21/03/10.
//  Copyright NetTech India 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CameraController.h"
#import "FaceAugmentingView.h"

@interface PictureMeController : UIViewController <UIImagePickerControllerDelegate, UINavigationControllerDelegate> {
    CameraController *camera;
    //FaceAugmentingView* fav;
    UIActivityIndicatorView *activity;
    UILabel *activityLabel;
}

@property (nonatomic, retain) CameraController *camera;
//@property (nonatomic, retain) FaceAugmentingView* fav;
@property (nonatomic, retain) IBOutlet UIActivityIndicatorView *activity;
@property (nonatomic, retain) IBOutlet UILabel *activityLabel;

- (void)savedImage:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo;

@end

