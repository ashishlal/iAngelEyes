//
//  iAngelEyesImageView.h
//  iAngelEyes
//
#import <UIKit/UIKit.h>


@interface iAngelEyesImageView : UIView {
    CGRect face;
    UIImage *image;
	
}


@property (nonatomic, assign) CGRect face;
@property (nonatomic, retain) UIImage *image;


@end
