//
//  iAngelEyesController.h
//  iAngelEyes

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "UIImage-Additions.h"
#import "iAngelEyesCameraBar.h"
#import "iAngelEyesPreviewBar.h"
#import "iAngelEyesPreviewBarMore.h"
#import "iAngelEyesImageView.h"
#import "FaceAugmentingView.h"
#import "InfoViewController.h"
#import "iAngelEyesBrowseBar.h"
#import "iAngelEyesColorBar.h"
#import "iAngelEyesColorBarMore.h"
#import "iAngelEyesLightBar.h"
#import "iAngelEyesPreviewBarCV.h"

@class GLView;
@class InfoViewController;

@interface iAngelEyesController : UIViewController <UIImagePickerControllerDelegate, UINavigationControllerDelegate, UIAlertViewDelegate> {
   
	UIDeviceOrientation orientation;
    UIImagePickerController *camera;
    iAngelEyesImageView *imageView;
	iAngelEyesImageView *textureImageView;
	UIImageView *coverImageView;
    UIImage *image;
	UIImage *croppedNoseImage;
    
#if 0   
    CvHaarClassifierCascade *model;
	CvHaarClassifierCascade *nose_model;
	CvHaarClassifierCascade *mouth_model;
	CvHaarClassifierCascade *leye_model;
	CvHaarClassifierCascade *reye_model;
	CvHaarClassifierCascade *profile_face_model;
#endif	
	NSString *face_model_file;
    NSString *nose_model_file;
	NSString *mouth_model_file;
	NSString *leye_model_file;
	NSString *reye_model_file;
	NSString *profile_face_model_file;
	
	UIActivityIndicatorView *cLoadingView;
	GLView *glView;
	FaceAugmentingView* fav;
    
    iAngelEyesCameraBar		*camerabar;
    iAngelEyesPreviewBar	*previewbar;
	iAngelEyesPreviewBarMore *previewbarMore;
	iAngelEyesBrowseBar		*browsebar;
	UISlider				*slider;
	iAngelEyesColorBar		*colorbar;
	iAngelEyesColorBarMore	*colorbarMore;
	iAngelEyesLightBar		*lightbar;
	iAngelEyesPreviewBarCV	*previewbarCV;
	UILabel					*noseNameDisplay;
	
	UINavigationController *naviController;
    InfoViewController *infoController;
	
	UIProgressView *progbar;
	UIActionSheet *alert;
	//UIImage *viewImage;
	//UIImage *viewImage2;
	//UIImage *viewImage3;
	UIImageView *imgView;
	
	CGRect face;
	int amt;
	BOOL isPreview;
    BOOL isDisplayed;
    BOOL detecting;
	
	BOOL iAmUpdating;
	
	int matProperty;
	float m_ambience;
	float m_diffuse;
	float m_specular;
	float m_shininess;
	int noseIndex;
	unsigned int appState;
	int m_loopImageCapture;
	int m_cameraButtonPressed;
	int m_featureOutlineEnabled;
	int m_retries;
	int m_disableFaceDetect;
	int m_cameraRoll;
	int m_useQuickestSearch;
	int m_preserveOrientation;
	int m_maxAttempts;
	unsigned int m_initModels;
	int m_useCameraView;
	int m_cameraBarInFront;
	float m_resizeVal;
	int m_resize;
}


@property (nonatomic, retain) UIImagePickerController *camera;
@property (nonatomic, retain) iAngelEyesImageView *imageView;
@property (nonatomic, retain) iAngelEyesImageView *textureImageView;
@property (nonatomic, retain) UIImageView *coverImageView;
@property (nonatomic, retain) UIImage *image;
@property (nonatomic, retain) UIImage *croppedNoseImage;
@property (nonatomic, assign) NSString *face_model_file, *nose_model_file, *mouth_model_file, *leye_model_file, *reye_model_file, *profile_face_model_file;
@property (nonatomic, retain) UIActivityIndicatorView *cLoadingView;
@property (nonatomic, retain) GLView *glView;
@property (nonatomic, retain) FaceAugmentingView* fav;
@property (nonatomic, retain) iAngelEyesCameraBar		*camerabar;
@property (nonatomic, retain) iAngelEyesPreviewBar		*previewbar;
@property (nonatomic, retain) iAngelEyesPreviewBarMore *previewbarMore;
@property (nonatomic, retain) iAngelEyesBrowseBar		*browsebar;
@property (nonatomic, retain) UISlider *slider;
@property (nonatomic, retain) iAngelEyesColorBar		*colorbar;
@property (nonatomic, retain) iAngelEyesColorBarMore	*colorbarMore;
@property (nonatomic, retain) iAngelEyesLightBar		*lightbar;
@property (nonatomic, retain) iAngelEyesPreviewBarCV		*previewbarCV;
@property (nonatomic, retain) UILabel *noseNameDisplay;
@property (nonatomic, retain) UINavigationController *naviController;
@property (nonatomic, retain) InfoViewController *infoController;

//@property (nonatomic, assign) CvHaarClassifierCascade *model, *nose_model, *mouth_model, *leye_model, *reye_model, *profile_face_model;
//@property (nonatomic, retain) OpenGLWaveFrontObject *nose;


@property (nonatomic, retain) UIProgressView *progbar;
@property (nonatomic, retain) UIActionSheet *alert;

//@property (nonatomic, retain) UIImage *viewImage;
//@property (nonatomic, retain) UIImage *viewImage2;
//@property (nonatomic, retain) UIImage *viewImage3;

@property (nonatomic, retain) UIImageView *imgView;
//@property (nonatomic, retain) UIImageView *imgView2;
//@property (nonatomic, retain) UIImageView *imgView3;

@property (nonatomic, assign) CGRect face;
@property (nonatomic, assign) int amt;
@property (nonatomic, assign) BOOL isPreview;
@property (nonatomic, assign) BOOL isDisplayed;
@property (nonatomic, assign) BOOL detecting;

@property (nonatomic, assign) BOOL iAmUpdating;
@property (nonatomic, assign) int matProperty;
@property (nonatomic, assign) float m_ambience;
@property (nonatomic, assign) float m_diffuse;
@property (nonatomic, assign) float m_specular;
@property (nonatomic, assign) float m_shininess;
@property (nonatomic, assign) int noseIndex;
@property (nonatomic, assign) unsigned int appState;
@property (nonatomic, assign) int m_loopImageCapture;
@property (nonatomic, assign) int m_cameraButtonPressed;
@property (nonatomic, assign) int m_featureOutlineEnabled;
@property (nonatomic, assign) int m_retries;
@property (nonatomic, assign) int m_disableFaceDetect;
@property (nonatomic, assign) int m_cameraRoll;
@property (nonatomic, assign) int m_useQuickestSearch;
@property (nonatomic, assign) int m_preserveOrientation;
@property (nonatomic, assign) int m_maxAttempts;
@property (nonatomic, assign) unsigned int m_initModels;
@property (nonatomic, assign) int m_useCameraView;
@property (nonatomic, assign) int m_cameraBarInFront;
@property (nonatomic, assign) float m_resizeVal;
@property (nonatomic, assign) int m_resize;

- (void)drawView:(GLView*)view;
//- (void)setupView:(GLView*)view;
- (void)setupPos:(GLView*)view;
- (BOOL) AmIUpdating;

- (void)setNextScreen:(UIImage *)aImage;

- (void)savedImage:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo;
- (void) delayedDismissOfModalViewController:(id)picker;
- (void) photoLibrary;
- (void) showInfoPanel;
- (void) showInfoPanelAsFirstTimeLaunch:(BOOL)firstTimeLaunch;
- (void) setNavController;
- (void) dismissInfoScreen;
- (void)takePicture;
- (void)retakePicture;
- (void)showConfirmAlert:(NSString *)str;
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
- (void)usePicture;
- (void)movePicture;
- (void)rotatePicture;
- (void)goToPreviewBarMore;
- (void)browseObjects;
- (void)firstObject;
- (void)prevObject;
- (void)nextObject;
- (void)lastObject;
-(void)setTexture:(GLView*)view;
- (void)ObjectTexture; 
- (void)ObjectAmbience; 
- (void)ObjectDiffusion ;
- (void)ObjectSpecular; 
- (void)ObjectShininess; 
- (void)ObjectColorReset; 
- (void)goToMaterialProperties;
- (void)goToMaterialPropertiesMore;
- (void)goBackToPreviewBar;
- (void)goToLightBar;
- (void) handleSlider:(UISlider*)sender;
- (void)resizeObject;
- (UIColor *) naviBarTintColor;
- (void)updateStatusDisplay:( NSString *)str;
- (void) UpdateObjectDisplayName:( NSString *)str;
- (void)startDetection;
- (void)stopDetection;
- (void)initSpinner;
- (void)spinBegin;
- (void)spinEnd;
- (void) InitProgressBar;
- (void) handleTimer: (id) timer;
- (void) StopProgressBar;
- (void) SetAlphaCorrection: (int)val;
- (void) Set3GSFactor: (int)val;
- (void) SetLoopImageCapture: (int)val;
- (void) ShowFeatureOutlines: (int)val;
- (void) UseCameraAR: (int)val;
- (void) ToggleLight: (int) val;
- (void)useQuickestSearch:(int)v;
- (void)preserveOrientation:(int)v;
- (void)disableFDModule:(int)v;
- (void)maxAttemptsSet:(int)v;
- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image;
- (int)dateExpired;
@end
