//
//  UIImageOpenCV.h
//

#import <Foundation/Foundation.h>
#import "cv.h"

@interface UIImage (OpenCV) 

+ (UIImage *)imageWithCVImage:(IplImage *)cvImage;

- (IplImage *)cvImage;

- (IplImage *)cvGrayscaleImage;

- (IplImage *)cvGrayscaleImageMY;

@end
