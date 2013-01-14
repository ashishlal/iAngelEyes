//
//  CameraController.h
//  TouchCamera
//
//  Created by Jeremy Collins on 3/11/09.
//  Copyright 2009 Jeremy Collins. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "OpenGLWaveFrontObject.h"
#import "UIImageAdditions.h"
#import "FaceAugmentingView.h"
#import "camshift_wrapper.h"
//#import "ml.h"

@class GLView;

//@protocol UIImagePickerControllerDelegate;
//@class CvNormalBayesClassifier;
#define MAX_RECT 10

@interface CameraController : UIImagePickerController {
    NSTimer *faceTimer;
    NSTimer *previewTimer;
        
    CvHaarClassifierCascade *model;
	CvHaarClassifierCascade *nose_model;
	CvHaarClassifierCascade *mouth_model;
	CvHaarClassifierCascade *leye_model;
	CvHaarClassifierCascade *reye_model;
	//MyBayesClassifierWrapper *bayes;
	
	//OpenGLWaveFrontObject *plane;
	//OpenGLWaveFrontObject *cylinder;
	//OpenGLWaveFrontObject *cone;
	OpenGLWaveFrontObject *nose;
	
	NSString *face_model_file;
	NSString *nose_model_file;
	NSString *mouth_model_file;
	NSString *leye_model_file;
	NSString *reye_model_file;
	GLView *glView;
	
    UIView *camera;
    FaceAugmentingView* fav;
    BOOL isPreview;
    BOOL isDisplayed;
    
    UIDeviceOrientation o;
}

@property (nonatomic, retain) NSTimer *faceTimer;
@property (nonatomic, retain) NSTimer *previewTimer;
@property (nonatomic, assign) CvHaarClassifierCascade *model, *nose_model, *mouth_model, *leye_model, *reye_model;
//@property (nonatomic, assign) CvNormalBayesClassifer *bayes;
//@property (nonatomic, retain) OpenGLWaveFrontObject *plane;
//@property (nonatomic, retain) OpenGLWaveFrontObject *cylinder;
//@property (nonatomic, retain) OpenGLWaveFrontObject *cone;
@property (nonatomic, retain) OpenGLWaveFrontObject *nose;
@property (nonatomic, retain) GLView *glView;
@property (nonatomic, assign) NSString *face_model_file, *nose_model_file, *mouth_model_file, *leye_model_file, *reye_model_file;


- (void)drawView:(GLView*)view;
- (void)setupView:(GLView*)view;
-(void)setupPos:(GLView*)view;

#ifdef NOT_USED
-(void) populateFeatureData:(CvMat* )train responses:(CvMat *)responses iFeatureType:(int)iFeatureType size:(int)size;
- (void) myInitNormalBayesClassifier;
- (void) myCompareImages: (IplImage *)img;
#endif

+ (CameraController *)instance;

@end
