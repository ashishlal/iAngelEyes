//
//  PMToolbar.h
//  iAngelEyes
//

#import <UIKit/UIKit.h>


@class iAngelEyesController;


@interface iAngelEyesCameraBar : UIToolbar {
    iAngelEyesController *delegate;
	
	UIButton *cameraButton;
	UIButton *infoButton;
	UILabel *statusLabel;
}


@property (nonatomic, assign) iAngelEyesController *delegate;

@property (nonatomic, retain) UIButton *cameraButton;
@property (nonatomic, retain) UIButton *infoButton;
@property (nonatomic, retain) UILabel *statusLabel;

@end
