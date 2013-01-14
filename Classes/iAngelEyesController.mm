//
//  iAngelEyesController.m
//  iAngelEyes
//

#import <QuartzCore/QuartzCore.h>
#import <AudioToolbox/AudioServices.h>
#import "UIImage-Additions.h"
#import "UIDevice-Hardware.h"
#import "iAngelEyesController.h"
#import "UIImage-Additions.h"
#import "GLView.h"
#import "InfoViewController.h"
#import "ConstantsAndMacros.h"
//#include "TargetConditionals.h"
#include "NTIMalloc.h"

//extern "C" void detect_and_draw( IplImage* img, CvRect* found_face, CvSize s );

extern "C" void init_detection(char* cascade_location);
extern "C" void init_detection_multiple(char* loc0, char* loc1, char* loc2, char* loc3, char *loc4, char* loc5, 
										int flag, int searchFlag);
extern "C" void release_detection();
extern "C" void reinit_detection(char* cascade_location);
extern "C" void release_detection_multiple() ;

extern "C" void detect_multiple( IplImage *img, 
								CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
								CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
								CvSize sz, int *);
extern "C" void detect_profileface_multiple( IplImage *img, 
								CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
								CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
								CvSize sz, int *);
extern "C" CGImageRef UIGetScreenImage();


extern CGImageRef UIGetScreenImage();
static CvMemStorage *storage = 0;

extern int g_focus_on_image;
extern "C" double g_using_scaled_image;

@implementation iAngelEyesController

@synthesize camera;
@synthesize imageView;
@synthesize textureImageView;
@synthesize coverImageView;
@synthesize image;
@synthesize croppedNoseImage;
@synthesize face_model_file, nose_model_file, mouth_model_file, leye_model_file, reye_model_file, profile_face_model_file;
@synthesize cLoadingView;
@synthesize glView;
@synthesize fav;
@synthesize camerabar;
@synthesize previewbar;
@synthesize previewbarMore;
@synthesize browsebar;
@synthesize slider;
@synthesize colorbar;
@synthesize colorbarMore;
@synthesize lightbar;
@synthesize previewbarCV;
@synthesize noseNameDisplay;
@synthesize naviController, infoController;
@synthesize alert;
@synthesize progbar;
@synthesize imgView;
@synthesize face;
@synthesize amt;
@synthesize isPreview, isDisplayed, detecting, iAmUpdating;
@synthesize matProperty;
@synthesize m_ambience;
@synthesize m_diffuse;
@synthesize m_specular;
@synthesize m_shininess;
@synthesize noseIndex;
@synthesize appState;
//@synthesize viewImage, viewImage2, viewImage3, imgView, imgView2, imgView3;
@synthesize m_loopImageCapture, m_cameraButtonPressed, m_featureOutlineEnabled, m_retries, m_disableFaceDetect, m_cameraRoll;
@synthesize m_useQuickestSearch, m_preserveOrientation, m_maxAttempts;
@synthesize m_initModels;
@synthesize m_useCameraView;
@synthesize m_cameraBarInFront;
@synthesize m_resizeVal;
@synthesize m_resize;

char *chars1 =nil, *chars2=nil, *chars3=nil, *chars4=nil, *chars5=nil, *chars6=nil;

CvRect *r[MAX_RECT];
CvRect *rNose[MAX_RECT];
CvRect *rMouth[MAX_RECT];
CvRect *rEyes[MAX_RECT];

CvRect *unscaled_r[MAX_RECT];
CvRect *unscaled_rNose[MAX_RECT];
CvRect *unscaled_rMouth[MAX_RECT];
CvRect *unscaled_rEyes[MAX_RECT]; 


- (void)dealloc {
	
    self.camera = nil;
	
#if 0	
	if(self.imageView) [imageView release];
	if(self.textureImageView) [textureImageView release];
	if(self.coverImageView) [textureImageView release];
	//if(self.image) [textureImageView release];
	if(self.croppedNoseImage)
	if(self.face_model_file) 
	if(nose_model_file) 
	if(mouth_model_file) 
	if(leye_model_file) 
	if(reye_model_file) 
	if(profile_face_model_file)
	if(self.cLoadingView)
	if(self.glView)
	if(self.fav)
	
	self.camerabar = nil;
	self.previewbar = nil;
	self.previewbarMore = nil;
	self.browsebar = nil;
	self.colorbar = nil;
	self.colorbarMore = nil;
	self.lightbar =nil;
#endif	
    release_detection_multiple();
	if(chars1) free(chars1);
	if(chars2) free(chars2);
	if(chars3) free(chars3);
	if(chars4) free(chars4);
	if(chars5) free(chars5);
	if(chars6) free(chars6);
	
	for (int j=0; j < MAX_RECT; j++) {
		if(r[j]) free((void *)r[j]);
		if(rNose[j]) free((void *)rNose[j]);
		if(rMouth[j]) free((void *)rMouth[j]);
		if(rEyes[j]) free((void *)rEyes[j]);
		if(unscaled_r[j]) free((void *)unscaled_r[j]);
		if(unscaled_rNose[j]) free((void *)unscaled_rNose[j]);
		if(unscaled_rMouth[j]) free((void *)unscaled_rMouth[j]);
		if(unscaled_rEyes[j]) free((void *)unscaled_rEyes[j]);
		
	}
	
	if(glView) [glView release];
	
    [super dealloc];
}


- (void)viewDidLoad {
	
	appState = INITIALZING;
#ifndef TARGET_IPHONE_SIMULATOR	
    self.camera = [[UIImagePickerController alloc] init];
    camera.sourceType = UIImagePickerControllerSourceTypeCamera;
    camera.delegate = self;
	//camera.allowsImageEditing = YES;
    camera.showsCameraControls = NO;
    camera.cameraOverlayView = self.view;
	//[self.view addSubview:camera.view];
#endif
    camerabar = [[iAngelEyesCameraBar alloc] initWithFrame:CGRectMake(0, 480 - 53, 320, 53)];
    camerabar.delegate = self;
	camerabar.tag = CAMERA_BAR_TAG;
    [self.view addSubview:camerabar];
	//[self.view bringSubviewToFront:camerabar];
    [camerabar release];
    
    previewbar = [[iAngelEyesPreviewBar alloc] initWithFrame:CGRectMake(320, 480 - 53, 320, 53)];
    previewbar.delegate = self;
	previewbar.tag = PREVIEW_BAR_TAG;
    [self.view addSubview:previewbar];
    [previewbar release];

	previewbarCV = [[iAngelEyesPreviewBarCV alloc] initWithFrame:CGRectMake(320, 480 - 53, 320, 53)];
    previewbarCV.delegate = self;
	previewbarCV.tag = LIGHT_BAR_TAG;
    [self.view addSubview:previewbarCV];
	[previewbarCV release];

	previewbarMore = [[iAngelEyesPreviewBarMore alloc] initWithFrame:CGRectMake(640, 480 - 53, 320, 53)];
    previewbarMore.delegate = self;
	previewbarMore.tag = PREVIEWMORE_BAR_TAG;
    [self.view addSubview:previewbarMore];
	[previewbarMore release];
	
	browsebar = [[iAngelEyesBrowseBar alloc] initWithFrame:CGRectMake(960, 480 - 53, 320, 53)];
    browsebar.delegate = self;
	browsebar.tag = BROWSE_BAR_TAG;
    [self.view addSubview:browsebar];
	[browsebar release];
	
	self.noseNameDisplay = [[UILabel alloc] initWithFrame:CGRectMake(960, 387, 320, 40)];
	noseNameDisplay.backgroundColor = [UIColor clearColor];
	noseNameDisplay.textAlignment = UITextAlignmentCenter;
	noseNameDisplay.font = [UIFont boldSystemFontOfSize:15.0];
	noseNameDisplay.tag = NOSE_DISPLAY_TAG;
	[self.view addSubview:noseNameDisplay];
	[noseNameDisplay release];
	
	slider = [[UISlider alloc] initWithFrame: CGRectMake(1280, 480 - 90, 300, 20)];
	slider.minimumValue = 0.0;
	slider.maximumValue = 100.0;
	slider.tag = 0;
	slider.value = 50;
	slider.continuous = YES;
	slider.enabled = YES;
	[slider addTarget:self action:@selector(handleSlider:) forControlEvents:UIControlEventValueChanged];
	slider.tag = SLIDER_TAG;
	[self.view addSubview:slider];
	[slider release];	
	
	colorbar = [[iAngelEyesColorBar alloc] initWithFrame:CGRectMake(1280, 480 - 53, 320, 53)];
    colorbar.delegate = self;
	colorbar.tag = COLOR_BAR_TAG;
    [self.view addSubview:colorbar];
	[colorbar release];
		
	colorbarMore = [[iAngelEyesColorBarMore alloc] initWithFrame:CGRectMake(1600, 480 - 53, 320, 53)];
    colorbarMore.delegate = self;
	colorbarMore.tag = COLOR_BAR_TAG;
    [self.view addSubview:colorbarMore];
	[colorbarMore release];
#if 0	
	lightbar = [[iAngelEyesLightBar alloc] initWithFrame:CGRectMake(1920, 480 - 53, 320, 53)];
    lightbar.delegate = self;
	lightbar.tag = LIGHT_BAR_TAG;
    [self.view addSubview:lightbar];
	[lightbar release];
#endif	
		
	[self initSpinner];
	
	iAmUpdating=NO;
	
	noseNameDisplay.textColor = [UIColor colorWithRed:1.0 green:0.0 blue:0.01 alpha:1.000];
	noseIndex = 0;
	//noseIndex = EURO_NOSE_FEM_AVG;
	m_cameraRoll = 0;
	m_loopImageCapture = 0;
	m_cameraButtonPressed = 0;
	m_featureOutlineEnabled = 0;
	m_retries=0;
	m_disableFaceDetect=0;
	m_useCameraView=0;
	m_cameraBarInFront=0;
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];  
    [[NSNotificationCenter defaultCenter] addObserver:self
											selector:@selector(deviceOrientationDidChange:)
											name:@"UIDeviceOrientationDidChangeNotification"
											object:nil];  
	
}

- (void)doit {
    [self presentModalViewController:camera animated:YES];  
}

- (void)viewDidAppear:(BOOL)animated {
  
#if 0
	if(m_cameraRoll) {
		[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
		[self performSelector:@selector(doit) withObject:nil afterDelay:0.0];
		[[UIApplication sharedApplication] endIgnoringInteractionEvents];
	}
	else {
		 [self presentModalViewController:camera animated:NO]; 
	}
#endif`
	[self presentModalViewController:camera animated:NO]; 
	
	imgView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"AngelEyes-Cover-2.png"]];
	[self.view addSubview:imgView];
	[self.view bringSubviewToFront:imgView];
	//[imgView release];
	
	chars1 = (char*)malloc(512); memset((void *)(chars1), 0, 512);
	chars2 = (char*)malloc(512); memset((void *)(chars2), 0, 512);
	chars3 = (char*)malloc(1024); memset((void *)(chars3), 0, 1024);
	chars4 = (char*)malloc(512); memset((void *)(chars4), 0, 512);
	chars5 = (char*)malloc(512); memset((void *)(chars5), 0, 512);
	chars6 = (char*)malloc(512); memset((void *)(chars6), 0, 512);
	
	for (int j=0; j < MAX_RECT; j++) {
		r[j] = (CvRect *)malloc(sizeof(CvRect));
		rNose[j] = (CvRect *)malloc(sizeof(CvRect));
		rMouth[j] = (CvRect *)malloc(sizeof(CvRect));
		rEyes[j] = (CvRect *)malloc(sizeof(CvRect));
		
		unscaled_r[j] = (CvRect *)malloc(sizeof(CvRect));
		unscaled_rNose[j] = (CvRect *)malloc(sizeof(CvRect));
		unscaled_rMouth[j] = (CvRect *)malloc(sizeof(CvRect));
		unscaled_rEyes[j] = (CvRect *)malloc(sizeof(CvRect));
		
		memset((void *)(r[j]), 0, sizeof(CvRect));
		memset((void *)(rNose[j]), 0, sizeof(CvRect));
		memset((void *)(rMouth[j]), 0, sizeof(CvRect));
		memset((void *)(rEyes[j]), 0, sizeof(CvRect));
		
	}
	fav = [[FaceAugmentingView alloc] initWithFrame:self.view.frame];
	
	fav.backgroundColor = [UIColor clearColor];
	//[self.view addSubview:fav];
	fav.totEyes=0;
	fav.totNoses=0;
	fav.totMouths=0;
	fav.totNoses=0;

#if 0
	face_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_frontalface_alt2.xml" ofType:@"gz"];
	[face_model_file getCString:chars1 maxLength:512 encoding:NSUTF8StringEncoding];
	
	nose_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_nose.xml" ofType:@"gz"];
	[nose_model_file getCString:chars2 maxLength:512 encoding:NSUTF8StringEncoding];
	
	mouth_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_mouth.xml" ofType:@"gz"];
	//mouth_model_file = [[NSBundle mainBundle] pathForResource:@"Mouth.xml" ofType:@"gz"];
	[mouth_model_file getCString:chars3 maxLength:512 encoding:NSUTF8StringEncoding];
	mouth_model_file = NULL;
	
	leye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_lefteye.xml" ofType:@"gz"];
	[leye_model_file getCString:chars4 maxLength:512 encoding:NSUTF8StringEncoding];
	
	reye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_righteye.xml" ofType:@"gz"];
	[reye_model_file getCString:chars5 maxLength:512 encoding:NSUTF8StringEncoding];
	
	init_detection_multiple(chars1, chars2, chars3, chars4, chars5, chars6, m_initModels, m_useQuickestSearch);

	image = nil;
	croppedNoseImage = nil;
	m_ambience = (0.2f* 100.0f);
	m_diffuse = (1.0f * 100.0f);
	m_specular = (1.0f * 100.0f);
	m_shininess = (0.0f * 100.0f);
#endif

	
	m_useQuickestSearch =1;
	m_preserveOrientation = 0;
	m_maxAttempts = 1;
	m_initModels = 0;
	m_resizeVal = 50;
	m_resize = 1;
	glView = [[GLView alloc] initWithFrame:self.view.frame];
	[glView setController:self];
	//NTIMallocCheck((char *)__FILE__, __LINE__);
    [self startDetection];

}

#if 0
- (void)imagePickerController:(UIImagePickerController *)picker 
        didFinishPickingImage:(UIImage *)aImage
                  editingInfo:(NSDictionary *)editingInfo {

    [self stopDetection];
    
    image = [aImage retain];
    
    CGImageRef screen = UIGetScreenImage();
	if(!imageView)
		imageView = [[iAngelEyesImageView alloc] initWithFrame:CGRectMake(0, 0, 320, 427)];
    imageView.image = [[UIImage imageWithCGImage:screen] imageCroppedToRect:CGRectMake(0, 0, 320, 427)];
   // imageView.face = face;
    CGImageRelease(screen);
	
	[imageView addSubview:fav];
	[imageView addSubview:glView];
	[self.view addSubview:imageView];
    [imageView release];

    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
    camerabar.frame = CGRectMake(-320, 480 - 53, 320, 53);
    previewbar.frame = CGRectMake(0, 480 - 53, 320, 53);
	previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
	browsebar.frame = CGRectMake(640, 480 - 53, 320, 53);
	noseNameDisplay.frame = CGRectMake(640, 387, 320, 40);
	slider.frame = CGRectMake(960, 480 - 90, 300, 20);
	colorbar.frame = CGRectMake(960, 480 - 53, 320, 53);
	colorbarMore.frame = CGRectMake(1280, 480 - 53, 320, 53);
	lightbar.frame = CGRectMake(1600, 480 - 53, 320, 53);
	
    [UIView commitAnimations];

   
}
#endif

- (BOOL) AmIUpdating {
	return iAmUpdating;
}

#if 0
- (void)animationDidStop:(NSString *)animationID
                finished:(NSNumber *)finished
                 context:(void *)context {
    //if ([animationID isEqualToString:@"removeFromSuperviewWithAnimation"]) {
    //    [camera.view removeFromSuperview];
    //}
}
#endif
- (void)setNextScreen:(UIImage *)aImage
{
	
    [self stopDetection];
	
	//[NSThread detachNewThreadSelector: @selector(spinEnd) toTarget:self withObject:nil];
	
	if(m_cameraRoll) {
		//if(imgView) [imgView removeFromSuperview];
	
		//CGRect frame = self.view.frame;
		// frame.origin = CGPointMake(0.0, self.view.bounds.size.height);
		//camera.view.frame = frame;
		//[self.view addSubview:camera.view]; 
	}
	if(!m_useCameraView) {
		if(fav) [imageView addSubview:fav];
		if (glView) [imageView addSubview:glView];
		[self UpdateObjectDisplayName:NULL];
		//[imageView addSubview:noseNameDisplay];
	
		[self.view addSubview:imageView];
		[imageView release];
	}
	else {
		if(fav) [self.view addSubview:fav];
		if (glView) [self.view addSubview:glView];
	}
	if (textureImageView) [textureImageView release];
	
	
	
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5f];
	
	//[UIView setAnimationDelegate:self];
	//[UIView setAnimationDidStopSelector:@selector(animationDidStop:finished:context:)];
	
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(0, 387, 320, 40);
		previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(640, 480 - 53, 320, 53);
		slider.frame = CGRectMake(960, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(1280, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(1600, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:imageView];
	}
	else {
		camerabar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(0, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(0, 387, 320, 40);
		previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(640, 480 - 53, 320, 53);
		slider.frame = CGRectMake(960, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(1280, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(1600, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:previewbarCV];
		
	}
	
	//frame.origin = CGPointZero;
	//camera.view.frame = frame;
	
	//[self.view bringSubviewToFront:noseNameDisplay];
    [UIView commitAnimations];

	
}


- (void) delayedDismissOfModalViewController:(id)picker {
	
    [picker dismissModalViewControllerAnimated:YES];
	[picker release];
}


- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
	
	[self performSelector: @selector(delayedDismissOfModalViewController:)
			   withObject: picker
			   afterDelay: 0.0];
	
	imgView = [[UIImageView alloc] initWithFrame:CGRectMake(0,0,320,(480-53))];
	imgView.image = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
		//[picker dismissModalViewControllerAnimated:YES];
	
	//imgView.image =img;
	[self.view addSubview:imgView];
	
	m_cameraButtonPressed = 1;
	picker.view.hidden = YES;
	imgView.hidden = NO;
    [self.view bringSubviewToFront:imgView];
	[imgView retain];
	[imgView.image retain];
	[picker retain];
	[self startDetection];
}

- (void)imagePickerController:(UIImagePickerController *)picker
		didFinishPickingImage:(UIImage *)img editingInfo:(NSDictionary *)editingInfo {
	
	[self performSelector: @selector(delayedDismissOfModalViewController:)
			   withObject: picker
			   afterDelay: 0.0];
	//[picker dismissModalViewControllerAnimated:YES];
	imgView = [[UIImageView alloc] initWithFrame:CGRectMake(0,0,320,(480-53))];
	imgView.image =img;
	[self.view addSubview:imgView];
	m_cameraButtonPressed = 1;
	picker.view.hidden = YES;
	imgView.hidden = NO;
    [self.view bringSubviewToFront:imgView];
	[imgView retain];
	[img retain];
	[picker retain];
	
	[self startDetection];
}


- (void) photoLibrary {
	if(appState < READY) return;
	if(appState == PROCESSING) return;
	if(camera.sourceType != UIImagePickerControllerSourceTypeSavedPhotosAlbum) {
		camera.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
		[self presentModalViewController:camera animated:YES]; 
	}
	m_cameraRoll=1;
	
}

- (void) showInfoPanel {
	[self showInfoPanelAsFirstTimeLaunch:NO];
}

- (void) showInfoPanelAsFirstTimeLaunch:(BOOL)firstTimeLaunch
{
	int i=0;
	if(appState < READY) return;
	if(appState == PROCESSING) return;
	[self stopDetection];
	
	if(m_cameraBarInFront==1) {
		[self.view sendSubviewToBack:camerabar];
		m_cameraBarInFront = 0;
		[glView SetDraw:NO];
	}
	
	if (!infoController) {
		self.infoController = [[InfoViewController alloc] initWithNibName:@"InfoView" bundle:nil];
		infoController.delegate = self;
		[infoController view].tag = INFO_TAG;
		[self.view addSubview:[infoController view]];
		
	}
	
	infoController.lastCheck = [[NSUserDefaults standardUserDefaults] integerForKey:@"lastCheck"];
	infoController.lastUpdate = [[NSUserDefaults standardUserDefaults] integerForKey:@"lastUpdate"];
	infoController.firstTimeLaunch = firstTimeLaunch;
	
	
	if (!naviController) {
		naviController = [[UINavigationController alloc] initWithRootViewController:infoController];
		naviController.navigationBar.tintColor = [self naviBarTintColor];
		[naviController view].tag = NAVIGATION_TAG;
		//naviController.delegate = self;
		[self.view addSubview:[naviController view]];
	}

	
	
	for (UIView *checkView in [self.view subviews] ) {
		if (([checkView tag] == INFO_TAG) || ([checkView tag] == NAVIGATION_TAG)) {
			[[[self.view subviews] objectAtIndex:i] setHidden:NO];
			if(m_useCameraView) {
				[self.view bringSubviewToFront:checkView];
			}
		}
		i++;
	}
	
	
	
}

- (void)setNavController {
	
	if(!naviController) {
		naviController = [[UINavigationController alloc] initWithRootViewController:infoController];
		naviController.navigationBar.tintColor = [self naviBarTintColor];
		[naviController view].tag = NAVIGATION_TAG;
		//naviController.delegate = self;
		[self.view addSubview:[naviController view]];
	}
	
	
}

- (void)dismissInfoScreen {
	
	int i=0;
	
	for (UIView *checkView in [self.view subviews] ) {
		if (([checkView tag] == INFO_TAG) || ([checkView tag] == NAVIGATION_TAG)) {
			[[[self.view subviews] objectAtIndex:i] setHidden:YES];
			if(m_useCameraView) {
				[self.view sendSubviewToBack:checkView];
			}
		}
		i++;
	}
	if((!m_useCameraView) || (m_cameraBarInFront == 0)) {
		[self.view bringSubviewToFront:camerabar];
		m_cameraBarInFront=1;
	}
	if(appState == STOPPED) {
		appState = READY;
	}
		
}

- (void)takePicture {
	if(appState < READY) return;
	if(appState == PROCESSING) return;
   // [camera takePicture];
	if(camera.sourceType != UIImagePickerControllerSourceTypeCamera) {
		camera.sourceType = UIImagePickerControllerSourceTypeCamera;
		[self presentModalViewController:camera animated:NO]; 
	}
	m_cameraButtonPressed = 1;
	m_cameraRoll = 0;
#if 0
	CGImageRef screen = UIGetScreenImage();
	viewImage = [UIImage imageWithCGImage:screen];
	CGImageRef screen1 = UIGetScreenImage();
	viewImage2 = [UIImage imageWithCGImage:screen1];
	CGImageRef screen2 = UIGetScreenImage();
	viewImage3 = [UIImage imageWithCGImage:screen2];
	CGImageRelease(screen);
	CGImageRelease(screen1);
	CGImageRelease(screen2);
	
	[viewImage retain];
	[viewImage2 retain];
	[viewImage3 retain];
#endif
	
	[self startDetection];
}


- (void)retakePicture {
	
	if(!m_useCameraView) {
		if(image) [image release];
		//[imageView release];
		if (textureImageView) {
			[textureImageView removeFromSuperview];
		}
		[imageView removeFromSuperview];
		if(croppedNoseImage) [croppedNoseImage release];
		[glView SetDraw:NO];
		
	}
	else {
		return;
		if (textureImageView) {
			[textureImageView removeFromSuperview];
		}
		if(croppedNoseImage) [croppedNoseImage release];
		
		[glView ClearBuffer];
		//[glView SetDraw:NO];
		//[self.view sendSubviewToBack:glView];
	}
	
	[fav reset];
	[fav removeFromSuperview];
	//[fav release];
	fav=nil;
	m_resize=0;
	m_resizeVal = 50;
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	previewbar.frame = CGRectMake(640, 480 - 53, 320, 53);
    previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
    camerabar.frame = CGRectMake(0, 480 - 53, 320, 53);
	browsebar.frame = CGRectMake(320, 480 - 53, 320, 53);
	noseNameDisplay.frame = CGRectMake(320, 387, 320, 40);
	slider.frame = CGRectMake(640, 480 - 90, 300, 20);
	colorbar.frame = CGRectMake(640, 480 - 53, 320, 53);
	colorbarMore.frame = CGRectMake(960, 480 - 53, 320, 53);
	lightbar.frame = CGRectMake(1280, 480 - 53, 320, 53);
	
    [UIView commitAnimations];
	
    //previewbar.statusLabel.text = @"PREVIEW";
	camerabar.statusLabel.text = @"CLICK";
   // previewbar.useButton.enabled = YES;
   // previewbar.retakeButton.enabled = YES;
   // previewbar.moveButton.enabled = YES;
	//previewbar.rotateButton.enabled = YES;
	
	if(!m_loopImageCapture) m_cameraButtonPressed = 0;
	m_cameraRoll=0;
	m_retries = 0;
	if(!fav) {
		fav = [[FaceAugmentingView alloc] initWithFrame:self.view.frame];
		
		fav.backgroundColor = [UIColor clearColor];
		[fav toggleShowRect:m_featureOutlineEnabled];
	}
	if(m_useCameraView) {
		[self.view bringSubviewToFront:camerabar];
		m_cameraBarInFront=1;
		
	}
	appState = READY;
    //[self startDetection];
}

- (void)savedImage:(UIImage *)aImage didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
    //[self retakePicture];
}

- (void)showConfirmAlert:(NSString *)str
{
	UIAlertView *myalert = [[UIAlertView alloc] init];
	[myalert setTitle:@"Confirm"];
	[myalert setMessage:str];
	[myalert setDelegate:self];
	[myalert addButtonWithTitle:@"Yes"];
	[myalert addButtonWithTitle:@"No"];
	[myalert show];
	[myalert release];
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex 
{
    if (buttonIndex == 0)
	{
		CGImageRef screen = UIGetScreenImage();
		UIImage *myImg = [UIImage imageWithCGImage:screen];
		CGImageRelease(screen);
		CGRect scaled = CGRectMake(0, 0, 320, (480-53));
		UIImage *screenShot = [myImg imageCroppedToRect:scaled];
		
		CGRect rect = CGRectMake(0,0,320,480);
		
		UIGraphicsBeginImageContext(rect.size);
		[screenShot drawInRect:rect];
		UIImage *result = UIGraphicsGetImageFromCurrentImageContext();
		UIGraphicsEndImageContext();  
		
		UIImageWriteToSavedPhotosAlbum(result, nil, nil, nil);
		
	}
	else if (buttonIndex == 1)
	{
		// No
	}
}

- (void)usePicture {
    //previewbar.statusLabel.text = @"SAVING...";
    //previewbar.useButton.enabled = NO;
    //previewbar.retakeButton.enabled = NO;
	//previewbar.moveButton.enabled = NO;
	//previewbar.rotateButton.enabled = NO;
#if 1
	[self showConfirmAlert:@"Save to Photos Album?"];
#else
	UIImageWriteToSavedPhotosAlbum([glView getGLScreenshot], nil, nil, nil);
#endif
	
	
	// UIImageWriteToSavedPhotosAlbum(screenShot, self, @selector(savedImage:didFinishSavingWithError:contextInfo:), nil);
}

- (void)movePicture {
   // previewbar.statusLabel.text = @"MOVING...";
	//previewbar.useButton.enabled = YES;
   // previewbar.retakeButton.enabled = YES;
   // previewbar.moveButton.enabled = YES;
	//previewbar.rotateButton.enabled = YES;
	m_resize=0;
	[glView MoveObject];
}

- (void)rotatePicture {
    //previewbar.statusLabel.text = @"ROTATING...";
	//previewbar.useButton.enabled = YES;
    //previewbar.retakeButton.enabled = YES;
   // previewbar.moveButton.enabled = YES;
	//previewbar.rotateButton.enabled = YES;
	m_resize=0;
	[glView RotateObject];
}

- (void)goToPreviewBarMore {
	
	m_resize=0;
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-640, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(0, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(320, 387, 320, 40);
		slider.frame = CGRectMake(640, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(640, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(960, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(1280, 480 - 53, 320, 53);
	
	}
	else {
		camerabar.frame = CGRectMake(-640, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(0, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(320, 387, 320, 40);
		slider.frame = CGRectMake(640, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(640, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(960, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(1280, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:previewbarMore];
	}
	
	
    [UIView commitAnimations];
	
	
}

- (void)resizeObject {
	//[glView ResizeObject];
	m_resize=1;
	slider.value = m_resizeVal;
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	
	if(!m_useCameraView) {
		
		camerabar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(640, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(960, 480 - 53, 320, 53);
	}
	else {
		camerabar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(640, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:colorbar];
	}
	[self.view bringSubviewToFront:slider];
	[UIView commitAnimations];

}

- (void)browseObjects {
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(-640, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(0, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(0, 387, 320, 40);
		slider.frame = CGRectMake(320, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(640, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(960, 480 - 53, 320, 53);
		
	}
	else {
		camerabar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(-640, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(0, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(0, 387, 320, 40);
		slider.frame = CGRectMake(320, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(640, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:browsebar];
	}
	
	//[self.view bringSubviewToFront:noseNameDisplay];
    [UIView commitAnimations];
}


- (void)firstObject {
	//[self UpdateObjectDisplayName:@"LOADING]"];
	noseIndex = 0;
	[glView BrowseObjects:FIRST_OBJECT];
	[self UpdateObjectDisplayName:NULL];
}

- (void)prevObject {
	//[self UpdateObjectDisplayName:@"LOADING]"];
	
	if(noseIndex == 0)
		noseIndex = (MaxEyeSurfaceCount-1);
	else if(noseIndex > 0) noseIndex--;
	[glView BrowseObjects:PREV_OBJECT];
	[self UpdateObjectDisplayName:NULL];
}

- (void)nextObject {
	//[self UpdateObjectDisplayName:@"LOADING]"];
	noseIndex = (noseIndex + 1)%MaxEyeSurfaceCount;
	[glView BrowseObjects:NEXT_OBJECT];
	[self UpdateObjectDisplayName:NULL];
	
}

- (void)lastObject {
	//[self UpdateObjectDisplayName:@"LOADING]"];
	noseIndex = MaxEyeSurfaceCount-1;
	[glView BrowseObjects:LAST_OBJECT];
	[self UpdateObjectDisplayName:NULL];
}

-(void)setTexture:(GLView*)view
{
	//if (textureImageView) {
	//	[textureImageView removeFromSuperview];
	//}
	
	if(croppedNoseImage) [croppedNoseImage release];
	
	CGPoint texLoc = [glView GetTextureLoc];
	int noseW=32;
	int noseH=32;
	int noseX = texLoc.x-(noseW/2);
	int noseY = texLoc.y - (noseH/2);
#if 0
	unsigned long nearestPOW2 = (noseW < noseH)?noseW:noseH;
	nearestPOW2--;
	nearestPOW2 |= nearestPOW2 >> 1;
	nearestPOW2 |= nearestPOW2 >> 2;
	nearestPOW2 |= nearestPOW2 >> 4;
	nearestPOW2 |= nearestPOW2 >> 8;
	nearestPOW2 |= nearestPOW2 >> 16;
	nearestPOW2++;
	
	if(nearestPOW2 > 64) nearestPOW2= 64;
	else if(nearestPOW2 < 32) nearestPOW2 = 32;
#else
	unsigned long nearestPOW2 = noseW;
#endif
	
	CGRect texRect = CGRectMake(noseX, noseY, nearestPOW2, nearestPOW2);
	if(!m_useCameraView) {
		//unsigned long v=64;
		croppedNoseImage = [image imageCroppedToRect:texRect];
		
	}
	else {

		CGImageRef screen = UIGetScreenImage();
		UIImage *img = [UIImage imageWithCGImage:screen];
		CGImageRelease(screen);
		
		croppedNoseImage = [img imageCroppedToRect:texRect];
	}
	
	
	[croppedNoseImage retain];
	textureImageView.image = croppedNoseImage;
	
	//textureImageView = [[UIImageView alloc] initWithImage:croppedNoseImage]; 
	//[imageView addSubview:textureImageView];
	//[textureImageView retain];
	[glView setNoseTextureData:croppedNoseImage.CGImage w:nearestPOW2 h:nearestPOW2];
	
}

- (void)ObjectTexture {
	[glView ResetMovement];
	matProperty = TEXTURE;
	[glView setMatProperty:matProperty val:0];
	
}

- (void)ObjectAmbience {
	matProperty = AMBIENT;
	[glView ResetMovement];
	slider.value = m_ambience;
}

- (void)ObjectDiffusion {
	matProperty = DIFFUSE;
	[glView ResetMovement];
	slider.value = m_diffuse;
}

- (void)ObjectSpecular {
	matProperty = SPECULAR;
	[glView ResetMovement];
	slider.value = m_specular;
}

- (void)ObjectShininess {
	matProperty = SHININESS;
	[glView ResetMovement];
	slider.value = m_shininess;
}

- (void)ObjectColorReset {
	matProperty = NONE;
	[glView setMatProperty:matProperty val:0.0f];
}

- (void)goToMaterialProperties {
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5f];
	
	if(!m_useCameraView) {
		
		camerabar.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(640, 480 - 53, 320, 53);
	}
	else {
		camerabar.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(640, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:colorbar];
	}
	
	[self.view bringSubviewToFront:slider];
		
    [UIView commitAnimations];
	
}

- (void)goToMaterialPropertiesMore {
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5f];
	
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(320, 480 - 53, 320, 53);
	}
	else {
		camerabar.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(-960, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-640, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-320, 387, 320, 40);
		slider.frame = CGRectMake(0, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(320, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:colorbarMore];
	}
	
	[self.view bringSubviewToFront:slider];
	[self.view bringSubviewToFront:colorbarMore];

	
    [UIView commitAnimations];
	
}


- (void)goBackToPreviewBar {
	
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(640, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(640, 387, 320, 40);
		slider.frame = CGRectMake(960, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(1280, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(1600, 480 - 53, 320, 53);
	}
	else {
		camerabar.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(0, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(320, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(640, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(640, 387, 320, 40);
		slider.frame = CGRectMake(960, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(960, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(1280, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(1600, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:previewbar];
	}
	//[self.view bringSubviewToFront:noseNameDisplay];
    [UIView commitAnimations];
	
}

- (void)goToLightBar {
	
	[UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:.5f];
	
	if(!m_useCameraView) {
		camerabar.frame = CGRectMake(-1920, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(-1600, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-960, 387, 320, 40);
		slider.frame = CGRectMake(-640, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(-640, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(0, 480 - 53, 320, 53);
	}
	else {
		camerabar.frame = CGRectMake(-1920, 480 - 53, 320, 53);
		previewbarCV.frame = CGRectMake(-1600, 480 - 53, 320, 53);
		previewbarMore.frame = CGRectMake(-1280, 480 - 53, 320, 53);
		browsebar.frame = CGRectMake(-960, 480 - 53, 320, 53);
		noseNameDisplay.frame = CGRectMake(-960, 387, 320, 40);
		slider.frame = CGRectMake(-640, 480 - 90, 300, 20);
		colorbar.frame = CGRectMake(-640, 480 - 53, 320, 53);
		colorbarMore.frame = CGRectMake(-320, 480 - 53, 320, 53);
		previewbar.frame = CGRectMake(0, 480 - 53, 320, 53);
		[self.view bringSubviewToFront:previewbarCV];
	}
	
    [UIView commitAnimations];
	
}

- (void) handleSlider:(UISlider*)sender
{
	//if ([activeTextField canResignFirstResponder])
	//	[activeTextField resignFirstResponder];
	//NSLog(@"handleSlider: sender = %d, value = %.1f",  [sender tag], [sender value]);
	float v =[sender value];
	if(!m_resize) {
		[glView setMatProperty:matProperty val:v];
	
		switch (matProperty) {
			case AMBIENT:
				m_ambience = v;
				break;
			case DIFFUSE:
				m_diffuse = v;
				break;
			case SPECULAR:
				m_specular = v;
				break;
			case SHININESS:
				m_shininess = v;
				break;
			default:
				break;
		}
	}
	else {
		float scaleFactor = 1.0f;
		scaleFactor = (float)(v/m_resizeVal);
		m_resizeVal = v;
		[glView SetZoomFactor:scaleFactor];
	}
	
	return;
}

- (UIColor *) naviBarTintColor
{
	return [UIColor colorWithRed:0.0 green:0.25 blue:0.5 alpha:1.0];
}

-(void) updateStatusDisplay:( NSString *)str
{
	[camerabar.statusLabel performSelectorOnMainThread : @ selector(setText : ) withObject:str waitUntilDone:YES];
}

-(void) UpdateObjectDisplayName:( NSString *)str
{	
#if 0
	if(str==NULL) {

		switch (noseIndex) {
			case EURO_FEM_NOSE_BRIDGE_DEEP:
				noseNameDisplay.text = @"DEEP BRIDGE NOSE";
				break;
			case EURO_FEM_NOSE_BRIDGE_LONG:
				noseNameDisplay.text = @"LONG BRIDGE NOSE";
				break;
			case EURO_FEM_NOSE_BRIDGE_SHALLOW:
				noseNameDisplay.text = @"SHALLOW BRIDGE NOSE";
				break;
			case EURO_FEM_NOSE_BRIDGE_SHORT:
				noseNameDisplay.text = @"SHORT BRIDGE NOSE";
				break;
			case EURO_FEM_NOSE_DOWN:
				noseNameDisplay.text = @"NOSE DOWN";
				break;
			case EURO_FEM_NOSE_FLAT:
				noseNameDisplay.text = @"FLAT NOSE";
				break;
			case EURO_FEM_NOSE_LONG:
				noseNameDisplay.text = @"LONG NOSE";
				break;
			case EURO_FEM_NOSE_POINTED:
				noseNameDisplay.text = @"POINTED NOSE";
				break;
			case EURO_FEM_NOSE_REGION_CONCAVE:
				noseNameDisplay.text = @"CONCAVE NOSE";
				break;
			case EURO_FEM_NOSE_REGION_CONVEX:
				noseNameDisplay.text = @"CONVEX NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_DEEP:
				noseNameDisplay.text = @"DEEP SELLION NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_DOWN:
				noseNameDisplay.text = @"SELLION DOWN NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_SHALLOW:
				noseNameDisplay.text = @"SELLION SHALLOW NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_THIN:
				noseNameDisplay.text = @"SELLION THIN NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_UP:
				noseNameDisplay.text = @"SELLION UP NOSE";
				break;
			case EURO_FEM_NOSE_SELLION_WIDE:
				noseNameDisplay.text = @"SELLION WIDE NOSE";
				break;
			case EURO_FEM_NOSE_SHORT:
				noseNameDisplay.text = @"SHORT NOSE";
				break;
			case EURO_FEM_NOSE_TILT_DOWN:
				noseNameDisplay.text = @"TILT DOWN NOSE";
				break;
			case EURO_FEM_NOSE_TILT_UP:
				noseNameDisplay.text = @"TILT UP NOSE";
				break;
			case EURO_FEM_NOSE_UP:
				noseNameDisplay.text = @"NOSE UP NOSE";
				break;
			case EURO_FEM_NOSTRIL_TILT_DOWN:
				noseNameDisplay.text = @"NOSTRIL TILT DOWN";
				break;
			case EURO_FEM_NOSE_NOSTRIL_TILT_UP:
				noseNameDisplay.text = @"NOSTRIL TILT UP";
				break;
			case EURO_FEM_NOSTRILS_LARGE:
				noseNameDisplay.text = @"LARGE NOSTRILS";
				break;
			case EURO_FEM_NOSTRILS_SMALL:
				noseNameDisplay.text = @"SMALL NOSTRILS";
				break;
			case EURO_FEM_NOSTRILS_THIN:
				noseNameDisplay.text = @"THIN NOSTRILS";
				break;
			case EURO_FEM_NOSTRILS_WIDE:
				noseNameDisplay.text = @"WIDE NOSTRILS";
				break;
			case EURO_NOSE_AVG:
				noseNameDisplay.text = @"AVERAGE EUROPEAN NOSE (M)";
				break;
			case EURO_NOSE_BRIDGE_DEEP:
				noseNameDisplay.text = @"DEEP BRIDGE (M)";
				break;
			case EURO_NOSE_BRIDGE_LONG:
				noseNameDisplay.text = @"LONG BRIDGE (M)";
				break;
			case EURO_NOSE_BRIDGE_SHALLOW:
				noseNameDisplay.text = @"SHALLOW BRIDGE (M)";
				break;
				//case EURO_NOSE_BRIDGE_SHORT1:
				//	name = string("/euro-nose-bridge-short1.obj");
				//	break;
			case EURO_NOSE_BRIDGE_SHORT:
				noseNameDisplay.text = @"SHORT BRIDGE (M)";
				break;
			case EURO_NOSE_DOWN:
				noseNameDisplay.text = @"NOSE DOWN (M)";
				break;
			case EURO_NOSE_FEM_AVG:
				noseNameDisplay.text = @"AVERAGE EUROPEAN FEMALE NOSE";
				break;
			case EURO_NOSE_FLAT:
				noseNameDisplay.text = @"FLAT NOSE (M)";
				break;
			case EURO_NOSE_LONG:
				noseNameDisplay.text = @"LONG NOSE (M)";
				break;
			//case EURO_NOSE_MALE_AVG:
			//	noseNameDisplay.text = @"AVG MALE NOSE";
			//	break;
			//case EURO_NOSE_NOSTRIL_DOWN:
			//	noseNameDisplay.text = @"NOSTRIL DOWN (M)";
			//	break;
			case EURO_NOSE_NOSTRIL_THIN:
				noseNameDisplay.text = @"NOSTRIL THIN (M)";
				break;
			case EURO_NOSE_NOSTRIL_UP:
				noseNameDisplay.text = @"NOSTRIL UP (M)";
				break;
			case EURO_NOSE_NOSTRIL_WIDE:
				noseNameDisplay.text = @"NOSTRIL WIDE (M)";
				break;
			case EURO_NOSE_POINTED:
				noseNameDisplay.text = @"POINTED NOSE (M)";
				break;
			case EURO_NOSE_REGION_CONCAVE:
				noseNameDisplay.text = @"CONCAVE NOSE (M)";
				break;
			case EURO_NOSE_REGION_CONVEX:
				noseNameDisplay.text = @"CONVEX NOSE (M)";
				break;
			case EURO_NOSE_SELLION_DEEP:
				noseNameDisplay.text = @"SELLION DEEP NOSE (M)";
				break;
			case EURO_NOSE_SELLION_DOWN:
				noseNameDisplay.text = @"SELLION DOWN NOSE (M)";
				break;
			case EURO_NOSE_SELLION_SHALLOW:
				noseNameDisplay.text = @"SELLION SHALLOW NOSE (M)";
				break;
			case EURO_NOSE_SELLION_THIN:
				noseNameDisplay.text = @"SELLION THIN NOSE (M)";
				break;
			case EURO_NOSE_SELLION_UP:
				noseNameDisplay.text = @"SELLION UP NOSE (M)";
				break;
			case EURO_NOSE_SELLION_WIDE:
				noseNameDisplay.text = @"SELLION WIDE NOSE (M)";
				break;
			case EURO_NOSE_SHORT:
				noseNameDisplay.text = @"SHORT NOSE (M)";
				break;
			case EURO_NOSE_TILT_DOWN:
				noseNameDisplay.text = @"TILT DOWN NOSE (M)";
				break;
			case EURO_NOSE_TILT_UP:
				noseNameDisplay.text = @"TILT UP NOSE (M)";
				break;
			case EURO_NOSE_UP:
				noseNameDisplay.text = @"NOSE UP (M)";
				break;
			default:
				break;
		}
		//[noseNameDisplay performSelectorOnMainThread : @ selector(setText : ) withObject:noseNameDisplay.text  waitUntilDone:YES];
	}
	else {
		//[noseNameDisplay performSelectorOnMainThread : @ selector(setText : ) withObject:str  waitUntilDone:YES];
	}
	//[self.view bringSubviewToFront:noseNameDisplay];
#endif
	return ;
}

- (void)startDetection {
	if(m_cameraRoll) {
		[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
		[self performSelector:@selector(detectFaceThread) withObject:nil afterDelay:0.0];
		[[UIApplication sharedApplication] endIgnoringInteractionEvents];
	}
	else {
		[self performSelectorInBackground:@selector(detectFaceThread) withObject:nil];
	}
}

- (void)stopDetection {
	appState = STOPPED;
	self.detecting = NO;
}

- (void)initSpinner {
	
	cLoadingView = [[[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge] autorelease];    
	// we put our spinning "thing" right in the center of the current view
	CGPoint newCenter = (CGPoint) [self.view center];
	cLoadingView.center = newCenter;
	cLoadingView.tag = SPINNING_TAG;
	[self.view addSubview:cLoadingView];
	
}

- (void)spinBegin 
{
	[cLoadingView startAnimating];
}

- (void)spinEnd {
	[cLoadingView stopAnimating];
}

- (void)SetAlphaCorrection:(int)val 
{
	if(val) {
		[glView SetAlphaCorrection:1];
	}
	else {
		[glView SetAlphaCorrection:0];
	}

}

- (void)Set3GSFactor:(int)val 
{
	if(val) {
		g_using_scaled_image = 0.75f;
	}
	else {
		g_using_scaled_image = 0.5f;
	}
	
}

- (void)SetLoopImageCapture:(int)val 
{
	if(val) {
		m_loopImageCapture = 1;
	}
	else {
		m_loopImageCapture = 0;
	}
	
}

- (void)ShowFeatureOutlines:(int)val 
{
	m_featureOutlineEnabled = val;
	if(fav) [fav toggleShowRect:m_featureOutlineEnabled];
}

- (void)UseCameraAR:(int)val 
{
	if(val) {
		m_useCameraView = 1;
	}
	else {
		m_useCameraView = 0;
	}
}

- (void)ToggleLight:(int)val 
{
	[glView ToggleLight:val];
}

- (void)useQuickestSearch:(int)val 
{
	if(val ==0) 
		m_useQuickestSearch = 0;
	else 
		m_useQuickestSearch = 1;
}

- (void)preserveOrientation:(int)val 
{
	[glView PreserveOrientation:val];
}

- (void)disableFDModule:(int)val 
{
	if(val == 0) 
		m_disableFaceDetect = 0;
	else 
		m_disableFaceDetect=1;
}

- (void)maxAttemptsSet:(int)val {
	if(val == 0) 
		m_maxAttempts = 1024; // repeat a 1000 times
	else 
		m_maxAttempts = val;
}

#pragma mark Face Detection methods


- (void)deviceOrientationDidChange:(id)ignore {
    UIDevice *device = [UIDevice currentDevice];
    orientation = device.orientation;
}

- (IplImage *)CreateIplImageFromUIImage:(UIImage *)img {
	CGImageRef imageRef = img.CGImage;
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	IplImage *iplimage = cvCreateImage(cvSize(img.size.width, img.size.height), IPL_DEPTH_8U, 4);
	CGContextRef contextRef = CGBitmapContextCreate(iplimage->imageData, iplimage->width, iplimage->height,
													iplimage->depth, iplimage->widthStep,
													colorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault);
	CGContextDrawImage(contextRef, CGRectMake(0, 0, img.size.width, img.size.height), imageRef);
	CGContextRelease(contextRef);
	CGColorSpaceRelease(colorSpace);
	
	IplImage *ret = cvCreateImage(cvGetSize(iplimage), IPL_DEPTH_8U, 3);
	cvCvtColor(iplimage, ret, CV_RGBA2BGR);
	cvReleaseImage(&iplimage);
	
	return ret;
}

- (int)dateExpired
{
#if 0
	
	NSDate *today = [NSDate date];
	
	//NSDate * targetDate = [NSDate dateWithString:@"2010-07-09 19:29:54 -0400"]; 
	//NSTimeInterval expiryI = [targetDate timeIntervalSince1970];
	//NSLog(@"tI = %f", expiryI);
	NSTimeInterval expiry = 1278718194.0f;
	
	if ([today timeIntervalSince1970] > expiry) {
		UIAlertView *anAlert = [[UIAlertView alloc] initWithTitle:@"Expired" message:@"Please contact support@nettechind.com" delegate:self cancelButtonTitle:nil otherButtonTitles:nil];
		[anAlert show];
		//NSLog(@"expired");
		return 1;
		//[[UIApplication sharedApplication] terminate];
	}
	return 0;
#else
	return 0;
#endif
}

- (void)detectFaceThread {

	if(appState == STOPPED) return;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[self retain];

	self.detecting = YES;

	CGImageRef screen;
	UIImage *viewImage;
	CGImageRef screen1;
	UIImage *viewImage2;
	CGImageRef screen2;
	UIImage *viewImage3;
	int screenShotsTaken=0;
	
	m_useQuickestSearch = 0;
	if(!face_model_file) {
	//if(1) {
		int tempInitFlag=0;
		appState = INITIALZING;
		[self updateStatusDisplay:@"WAIT"];
		
		if(( m_cameraButtonPressed || m_loopImageCapture) && (!screenShotsTaken)) {
			screen = UIGetScreenImage();
			viewImage = [UIImage imageWithCGImage:screen];
			screen1 = UIGetScreenImage();
			viewImage2 = [UIImage imageWithCGImage:screen1];
			screen2 = UIGetScreenImage();
			viewImage3 = [UIImage imageWithCGImage:screen2];
		
			CGImageRelease(screen);
			CGImageRelease(screen1);
			CGImageRelease(screen2);
			
			screenShotsTaken=1;
		}
		
		if(!m_disableFaceDetect) {
			[NSThread detachNewThreadSelector: @selector(spinBegin) toTarget:self withObject:nil];
		
			tempInitFlag = (INIT_FACE_CASCADE | INIT_LEYE_CASCADE | INIT_REYE_CASCADE);
			m_initModels |= tempInitFlag;
		
			face_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_frontalface_alt2.xml" ofType:@"gz"];
			[face_model_file getCString:chars1 maxLength:512 encoding:NSUTF8StringEncoding];
			
			profile_face_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_profileface.xml" ofType:@"gz"];
			[profile_face_model_file getCString:chars6 maxLength:512 encoding:NSUTF8StringEncoding];
			
			nose_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_nose.xml" ofType:@"gz"];
			[nose_model_file getCString:chars2 maxLength:512 encoding:NSUTF8StringEncoding];
		
			mouth_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_mouth.xml" ofType:@"gz"];
			//mouth_model_file = [[NSBundle mainBundle] pathForResource:@"Mouth.xml" ofType:@"gz"];
			[mouth_model_file getCString:chars3 maxLength:512 encoding:NSUTF8StringEncoding];
			mouth_model_file = NULL;
			
			leye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_lefteye.xml" ofType:@"gz"];
			[leye_model_file getCString:chars4 maxLength:512 encoding:NSUTF8StringEncoding];
		
			reye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_righteye.xml" ofType:@"gz"];
			[reye_model_file getCString:chars5 maxLength:512 encoding:NSUTF8StringEncoding];
		
			init_detection_multiple(chars1, chars2, chars3, chars4, chars5, chars6, tempInitFlag, m_useQuickestSearch);
		
			free(chars2);
			free(chars3);
			free(chars4);
			free(chars5);
			free(chars1);
			free(chars6);
			
			image = nil;
			croppedNoseImage = nil;
			
			[NSThread detachNewThreadSelector: @selector(spinEnd) toTarget:self withObject:nil];
			[imgView removeFromSuperview];
		}
	}
	
	int expired = 0;
	expired = [self dateExpired];
	if(expired == 1)  return;
	
	if(!fav) {
		fav = [[FaceAugmentingView alloc] initWithFrame:self.view.frame];
		
		fav.backgroundColor = [UIColor clearColor];
		[fav toggleShowRect:m_featureOutlineEnabled];
	}
	
	appState = READY;
	[self updateStatusDisplay:@"CLICK"];

	if((m_loopImageCapture == 0) && (m_cameraButtonPressed == 0)) return;
	
	for (int j=0; j < MAX_RECT; j++) {
		memset((void *)(r[j]), 0, sizeof(CvRect));
		memset((void *)(rNose[j]), 0, sizeof(CvRect));
		memset((void *)(rMouth[j]), 0, sizeof(CvRect));
		memset((void *)(rEyes[j]), 0, sizeof(CvRect));
		
		memset((void *)(unscaled_r[j]), 0, sizeof(CvRect));
		memset((void *)(unscaled_rNose[j]), 0, sizeof(CvRect));
		memset((void *)(unscaled_rMouth[j]), 0, sizeof(CvRect));
		memset((void *)(unscaled_rEyes[j]), 0, sizeof(CvRect));
		
	}

	int eyes_det=0;
	int eyesW = 0;
	int eyesH = 0;
	int eyesX = 0 ;
	int eyesY = 0;
	double nScale = 1.50f;
	IplImage *snapshot = NULL, *snapshot1 = NULL;
	bool eyesDet=false;

	storage = NULL;
	//if(0) {
	

	CGRect scaled, tempScaled;
		
	if(m_cameraRoll) {
		CGImageRef cgImage = [imgView.image CGImage];
		
		// Make a new image from the CG Reference
		viewImage = [[UIImage alloc] initWithCGImage:cgImage];
		viewImage2 = [[UIImage alloc] initWithCGImage:cgImage];
		viewImage3 = [[UIImage alloc] initWithCGImage:cgImage];
	}		

#ifdef TARGET_IPHONE_SIMULATOR
	viewImage = [UIImage imageNamed:@"clooney.png"];
	viewImage2 = [UIImage imageNamed:@"clooney.png"];
	viewImage3 = [UIImage imageNamed:@"clooney.png"];
#else
		
	if((!m_cameraRoll) && ((m_cameraButtonPressed || m_loopImageCapture) && (!screenShotsTaken))) {
		screen = UIGetScreenImage();
		viewImage = [UIImage imageWithCGImage:screen];
		screen1 = UIGetScreenImage();
		viewImage2 = [UIImage imageWithCGImage:screen1];
		screen2 = UIGetScreenImage();
		viewImage3 = [UIImage imageWithCGImage:screen2];
			
		CGImageRelease(screen);
		CGImageRelease(screen1);
		CGImageRelease(screen2);
			
		screenShotsTaken=1;
	}
		
#endif
	if(!m_disableFaceDetect) {	
		
		//tempScaled = imgView2.frame;
		scaled.size = viewImage.size;
		tempScaled.size = viewImage.size;
	

#if 0  
	
		if([device platformType] != UIDevice3GSiPhone) {
			scaled.size.width *= .5;
			scaled.size.height *= .5;
			g_using_scaled_image = 0.5f;
		
		} else {
			scaled.size.width *= .75;
			scaled.size.height *= .75;
			g_using_scaled_image = 0.75f;
		}
#else
		if(g_using_scaled_image == 0.75) {
			scaled.size.width *= 0.75f;
			scaled.size.height *= 0.75f;
				
		}
		else {
			scaled.size.width *= 0.5f;
			scaled.size.height *= 0.5f;
			g_using_scaled_image = 0.5f;
		}
		
		UIDevice *device = [UIDevice currentDevice];
		if((g_using_scaled_image == 0.75f) && ([device platformType] != UIDevice3GSiPhone)) {
			scaled.size.width = tempScaled.size.width * 0.5f;
			scaled.size.height = tempScaled.size.height * 0.5f;
			g_using_scaled_image = 0.5f;
		}

#endif    
		
		
		fav.totFaces=0; fav.totNoses=0; fav.totMouths=0; fav.totEyes=0;
		int tot[4]={fav.totFaces, fav.totNoses, fav.totMouths, fav.totEyes};

		appState = PROCESSING;
		[self updateStatusDisplay:@"WAIT"];
		[NSThread detachNewThreadSelector: @selector(spinBegin) toTarget:self withObject:nil];
		//storage = cvCreateMemStorage(0);
#if 1
		viewImage = [viewImage scaleImage:scaled];
		//viewImage2 = [viewImage2 scaleImage:tempScaled];
		// Convert to grayscale and equalize.  Helps face detection.
		snapshot = [viewImage cvGrayscaleImage];
#else		
		snapshot = [self CreateIplImageFromUIImage:viewImage];
#endif
		if(!snapshot) return;
		
		detect_multiple(snapshot, r, rNose, rMouth, rEyes, tot, unscaled_r, unscaled_rNose,
					unscaled_rMouth, unscaled_rEyes, cvSize(20,20), &eyes_det);

		fav.totFaces = tot[0];
		int i=0;
		snapshot1 = NULL;
		
#if 0
		bool profileFace = false;
		if(0 == fav.totFaces) {
			int nose_det=0;
			//viewImage3 = [viewImage3 scaleImage:scaled];
			//snapshot1 = [viewImage3 cvGrayscaleImage];
			snapshot1 = [self CreateIplImageFromUIImage:viewImage3];
			if(!snapshot1) return;
			detect_profileface_multiple(snapshot1, r, rNose, rMouth, rEyes, tot, unscaled_r, unscaled_rNose,
						unscaled_rMouth, unscaled_rEyes, cvSize(20,20), &eyes_det);
			fav.totFaces = tot[0];
			if(fav.totFaces) 
			{
				profileFace = true; 
			}
		
		}
#endif		
		if(eyes_det) eyesDet = true;
		m_retries++;
		// for camera roll, just once and profile face just once
		if(m_cameraRoll) m_retries++;
		[NSThread detachNewThreadSelector: @selector(spinEnd) toTarget:self withObject:nil];
		if(fav.totFaces) {	

			appState = FACE_DETECTED;
		
			fav.faces[i] = CGRectMake(r[i]->x, r[i]->y, r[i]->width, r[i]->height);
		
			if(tot[3] > 0) fav.totEyes=tot[3];
			if(tot[3] == 0) {
				/* HACK: simply draw 2 eyes nose in middle of face */
				eyesDet = true;
				eyesW = 32;
				eyesH = 32;
				eyesX = (int)(((double)(unscaled_r[0]->width + unscaled_r[0]->x)/2.0f) * nScale);
				eyesY = (int)(((double)(unscaled_r[0]->height + unscaled_r[0]->y)/2.0f) * nScale);
				fav.totEyes = 2; 
				rEyes[0]->x = (int)(((double)(r[0]->width + r[0]->x)/3.0f) * 1.0f);
				rEyes[0]->y = (int)(((double)(r[0]->height + r[0]->y)/2.0f) * 1.0f);
				rEyes[0]->width = (int)(((double)(r[0]->width))/2.5f);
				rEyes[0]->height = (((double)(r[0]->width))/2.5f);
				
				rEyes[1]->x = (int)(((double)(rEyes[0]->x) + 96) * 1.0f);
				rEyes[1]->y = rEyes[0]->y;
				rEyes[1]->width = rEyes[0]->width;
				rEyes[1]->height = rEyes[0]->height;
				
				fav.eyes[0] = CGRectMake(rEyes[0]->x, rEyes[0]->y, rEyes[0]->width, rEyes[0]->height);
				fav.eyes[1] = CGRectMake(rEyes[1]->x, rEyes[1]->y, rEyes[1]->width, rEyes[1]->height);
			}
			else {
				for (i=0; i<tot[3]; i++) {
			
					fav.eyes[i] = CGRectMake(rEyes[i]->x, rEyes[i]->y, rEyes[i]->width, rEyes[i]->height);
					if(0 == i) {
						eyesDet = true;
						if(!g_focus_on_image) {	
							
							//if(eyes_det) {
							if(eyes_det) {
								eyesW = unscaled_rEyes[i]->width;
								eyesH = (unscaled_rEyes[i]->height)/2;
					
								eyesX = (int)(((double)(unscaled_rEyes[i]->x + unscaled_r[i]->x)) * nScale);
								eyesY = (int)(((double)(unscaled_rEyes[i]->y + unscaled_r[i]->y)) * nScale);
							}
							else {
								eyesW = 32;
								eyesH = 32;
								eyesX = (int)(((double)(unscaled_r[i]->width + unscaled_r[i]->x)/2.0f) * nScale);
								eyesY = (int)(((double)(unscaled_r[i]->height + unscaled_r[i]->y)/2.0f) * nScale);					
							}
					
						}
						else {
					
							eyesW = rEyes[i]->width;
							eyesH = (rEyes[i]->height)/2;
							eyesX = (int)(rEyes[i]->x) ;
							eyesY = (int)(rEyes[i]->y) ;
					
						}
					}
				}
			}
									
			//AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
			//[NSThread sleepForTimeInterval:1];
			
			
		} 
		else {
			eyesDet=false;
			[self updateStatusDisplay:@"CLICK"];
			[NSThread detachNewThreadSelector: @selector(spinEnd) toTarget:self withObject:nil];
		}
	}
	
	if((!eyesDet) && ((m_retries >= m_maxAttempts) || (m_disableFaceDetect))) {
		
		eyesDet = true;
		/* HACK: simply draw 2 eyes in middle of face */
		r[0]->x = 80;
		r[0]->y = 280;
		r[0]->width = 64;
		r[0]->height = 64;
		fav.faces[0] = CGRectMake(r[0]->x, r[0]->y, r[0]->width, r[0]->height);
		
		fav.totFaces = 1;
		fav.totEyes = 2; 
		rEyes[0]->x = (int)(((double)(r[0]->width + r[0]->x)/2.0f) * 1.0f);
		rEyes[0]->y = (int)(((double)(r[0]->height + r[0]->y)/2.0f) * 1.0f);
		
		rEyes[0]->width = (int)((double)(r[0]->width) * 0.75f);
		rEyes[0]->height = (int)((double)(r[0]->height) * 0.75f);
		
		rEyes[1]->x = (int)(((double)(rEyes[0]->x) + 96) * 1.0f);
		rEyes[1]->y = rEyes[0]->y;
		rEyes[1]->width = rEyes[0]->width;
		rEyes[1]->height = rEyes[0]->height;
		
		fav.eyes[0] = CGRectMake(rEyes[0]->x, rEyes[0]->y, rEyes[0]->width, rEyes[0]->height);
		fav.eyes[1] = CGRectMake(rEyes[1]->x, rEyes[1]->y, rEyes[1]->width, rEyes[1]->height);
	}
	if(((eyesDet) || (m_retries >= m_maxAttempts)) && (screenShotsTaken)) { 
		
		appState = NOSE_DETECTED;
		[self updateStatusDisplay:@"OK"];
#if 0		
		unsigned long nearestPOW2 = (eyesW < eyesH)?eyesW:eyesH;
		nearestPOW2--;
		nearestPOW2 |= nearestPOW2 >> 1;
		nearestPOW2 |= nearestPOW2 >> 2;
		nearestPOW2 |= nearestPOW2 >> 4;
		nearestPOW2 |= nearestPOW2 >> 8;
		nearestPOW2 |= nearestPOW2 >> 16;
		nearestPOW2++;
	
		if(nearestPOW2 > 64) nearestPOW2= 64;
		else if(nearestPOW2 < 32) nearestPOW2 = 64;
	
		
	
		//unsigned long v=64;
		CGRect texRect = CGRectMake(eyesX, eyesY, nearestPOW2, nearestPOW2);
		croppedNoseImage = [viewImage2 imageCroppedToRect:texRect];
	
		[croppedNoseImage retain];
		textureImageView = [[UIImageView alloc] initWithImage:croppedNoseImage]; 
		
		if(!m_useCameraView) {
			image = [viewImage2 retain];
			imageView = [[iAngelEyesImageView alloc] initWithFrame:CGRectMake(0, 0, 320, 427)];
			imageView.image = [[UIImage imageWithCGImage:image.CGImage] imageCroppedToRect:CGRectMake(0, 0, 320, 427)];
		
			[imageView addSubview:textureImageView];
		}
		else {
			[self.view addSubview:textureImageView];
		}
		//[textureImageView retain];
		//[textureImageView release];
		
		[glView setNoseTextureData:croppedNoseImage.CGImage w:nearestPOW2 h:nearestPOW2];
#else
		if(!m_useCameraView) {
			image = [viewImage2 retain];
			imageView = [[iAngelEyesImageView alloc] initWithFrame:CGRectMake(0, 0, 320, 427)];
			imageView.image = [[UIImage imageWithCGImage:image.CGImage] imageCroppedToRect:CGRectMake(0, 0, 320, 427)];
			
		}
#endif
		eyesDet=true;
		m_retries = 0;
		
		[self setNextScreen:viewImage2];
		
		[glView SetDraw:YES];
		[self setupPos:glView];
		
	}
	m_cameraButtonPressed =0;
	if((!eyesDet) && (m_retries < m_maxAttempts)) appState = READY;
	if(snapshot) cvReleaseImage(&snapshot);
	if(snapshot1) cvReleaseImage(&snapshot1);
	//if(storage) cvReleaseMemStorage(&storage);
	
	if((!eyesDet) && (self.detecting) && ((m_loopImageCapture) || (m_cameraButtonPressed == 1))) {
		if(m_cameraRoll) {
			[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
				[self performSelector:@selector(detectFaceThread) withObject:nil afterDelay:0.0];
			[[UIApplication sharedApplication] endIgnoringInteractionEvents];
		}
		else {
			 if(self.detecting) {
				 [self performSelectorInBackground:@selector(detectFaceThread) withObject:nil];
			 }
		}
	}
	
	
	[pool release];
	[self release];
	
	//NTIMallocCheck((char *)__FILE__, __LINE__);
	
}
		
- (void)drawView:(GLView*)view
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); 
	glColor4f(0.0, 0.5, 1.0, 1.0);
	
	glColor4f(1.0, 0.6, 0.3, 1.0);
}
	
	
#define MY_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define MAXY 120
#define MINY -80
#define MAXX 50
#define MINX 50
	
-(void)setupPos:(GLView *)view
{
	GLfloat x, y, z, w;
	int mynose=0, mymouth=1, myleye=2, myreye=3;
	int idx=mynose;
		//	if((!nose) || (!plane) || (!cylinder)) return;
	if(!view) return;

	fav.totMouths = fav.totNoses=0;
	if(fav.totNoses) {
			
		//x = view.frame.size.width - rNose[0]->x - rNose[0]->width;
		y = view.frame.size.height - rNose[0]->y - rNose[0]->height;
		x = rNose[0]->x;
		idx=mynose;
		z = (float)(rNose[0]->width);
		w = (float)(rNose[0]->height);
#ifdef DEBUG
		NSLog(@"nose-d0:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			  x,y,(float)(rNose[0]->width),(float)(rNose[0]->height));
#endif
		[view SetupScreenPos:idx x:x y:y z:z w:w];
	}
		
		
	if(fav.totMouths) {
			
			
		//x = view.frame.size.width - rMouth[0]->x - rMouth[0]->width;
		y = view.frame.size.height - rMouth[0]->y - rMouth[0]->height;
			
		NSLog(@"mouth-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			x,y,(float)(rMouth[0]->width),(float)(rMouth[0]->height));
			
		x = rMouth[0]->x;
		z = (float)(rMouth[0]->width);
		w = (float)(rMouth[0]->height);
		idx=mymouth;
		[view SetupScreenPos:idx x:x y:y z:z w:w];
	}
		
	if(fav.totEyes) {
#ifdef DEBUG			
		NSLog(@"eyes-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
				(float)(rEyes[0]->x),(float)(rEyes[0]->y),(float)(rEyes[0]->width),(float)(rEyes[0]->height));
#endif			
		//x = view.frame.size.width - rEyes[0]->x - rEyes[0]->width;
		y = view.frame.size.height - rEyes[0]->y - rEyes[0]->height;
		x = rEyes[0]->x;
#ifdef DEBUG
		NSLog(@"eyes0-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
				x,y,(float)(rEyes[0]->width),(float)(rEyes[0]->height));
#endif			
		idx=myleye;
		z = (float)(rEyes[0]->width);
		w = (float)(rEyes[0]->height);
		//[view SetupScreenPos:idx x:x y:y z:z w:w];
		[view SetupScreenPos:0 x:x y:y z:z w:w];
			
		//x = view.frame.size.width - rEyes[1]->x - rEyes[1]->width;
		y = view.frame.size.height - rEyes[1]->y - rEyes[1]->height;
		x = rEyes[1]->x;
#ifdef DEBUG
		NSLog(@"eyes1-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
				x,y,(float)(rEyes[1]->width),(float)(rEyes[1]->height));
#endif			
		idx=myreye;
		//z = (float)(rEyes[1]->width);
		//w = (float)(rEyes[1]->height);
		[view SetupScreenPos:1 x:x y:y z:z w:w];
	}
		
}

- (void) InitProgressBar
{
	//UIImageView *imgView = [[UIImageView alloc] initWithImage:@"Default.png"];
	self.alert = [[[UIActionSheet alloc] initWithTitle:@"Initializing.. Please Wait\n\n\n" delegate:nil 
								cancelButtonTitle:nil 
								destructiveButtonTitle: nil 
								otherButtonTitles: nil] autorelease];

	// Create the progress bar and add it to the alert	
    progbar = [[UIProgressView alloc] initWithFrame:CGRectMake(50.0f, 70.0f, 220.0f, 90.0f)];
	
    [alert addSubview:progbar];
	[progbar setProgressViewStyle: UIProgressViewStyleDefault];
	[progbar release];
	
    // Create the demonstration updates
    [progbar setProgress:(amt = 0.0f)];
	[NSTimer scheduledTimerWithTimeInterval: 0.35 target: self selector:@selector(handleTimer:) userInfo: nil repeats: YES];
	progbar.center = CGPointMake(progbar.center.x, progbar.center.y);
	
	[self.view addSubview:alert];
}
	
	
- (void) handleTimer: (id) timer
{
	amt += 1;
	[progbar setProgress: (amt / 20.0)];
	//if (amt > 20.0) {[alert dismiss]; [timer invalidate]; [alert removeFromSuperview]; amt = -1;}	
}

- (void) StopProgressBar
{
	if(amt == -1)  return;
	//amt = -1;
	//[alert dismiss]; 
	//[timer invalidate]; 
	//[alert removeFromSuperview];
}
	
	
#pragma mark UINavigationControllerDelegate Methods


- (void)navigationController:(UINavigationController *)navigationController didShowViewController:(UIViewController *)viewController animated:(BOOL)animated {
    
}


- (void)navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated {
    
}


@end
