//
//  iAngelEyesPreviewBar.h
//  iAngelEyes
//


#import <UIKit/UIKit.h>


@class iAngelEyesController;


@interface iAngelEyesPreviewBar : UIToolbar {
    iAngelEyesController *delegate;
	
	UIButton *retakeButton;
	UIButton *moveButton;
	UIButton *rotateButton;
	UIButton *resizeButton;
	UIButton *forwardButton;
    
}


@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *retakeButton;
@property (nonatomic, retain) UIButton *moveButton;
@property (nonatomic, retain) UIButton *rotateButton;
@property (nonatomic, retain) UIButton *resizeButton;
@property (nonatomic, retain) UIButton *forwardButton;

@end
