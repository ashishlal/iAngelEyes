//
//  UIImageResizing.h
//


#import <Foundation/Foundation.h>


@interface UIImage (Resize)


- (UIImage *)scaleImage:(CGRect)rect;

- (UIImage *) imageCroppedToRect:(CGRect)cropRect;

- (UIImage *)imageByCropping:toRect:(CGRect)rect;

- (void *)GetImageData;

//- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image;

@end