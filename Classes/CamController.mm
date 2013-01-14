//
//  CamController.m
//  iPictureMe
//
//  Created by Ashish Lal on 29/03/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "CameraController.h"
#import "GLView.h"
#import <QuartzCore/QuartzCore.h> 
#import "AudioToolbox/AudioServices.h"
#import "UIDeviceAdditions.h"
#import "DebugOutput.h"
//#import "ml.h"

#if 1
extern "C" void detect_and_draw( IplImage* img, CvRect* found_face, CvSize s );
extern "C" int detect_multiple1( IplImage* img, CvRect* found_face[], CvSize s, int feature );
extern "C" void init_detection(char* cascade_location);
extern "C" void init_detection_multiple(char* cascade_location, char* loc1, char* loc2, char* loc3, char *loc4);
extern "C" void release_detection();
extern "C" void reinit_detection(char* cascade_location);
extern "C" void release_detection_multiple() ;

extern "C" CvSeq* mycvHaarDetectObjects( const CvArr* _img,
										CvHaarClassifierCascade* cascade,
										CvMemStorage* storage, double scale_factor,
										int min_neighbors, int flags, CvSize min_size );
extern "C" void detect_multiple( IplImage *img, 
								CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
								CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
								CvSize sz);
extern "C" CGImageRef UIGetScreenImage();
extern "C" { 
	void myInitNormalBayesClassifier();
}

#else

void detect_and_draw( IplImage* img, CvRect* found_face, CvSize s );
int detect_multiple1( IplImage* img, CvRect* found_face[], CvSize s, int feature );
void init_detection(char* cascade_location);
void init_detection_multiple(char* cascade_location, char* loc1, char* loc2, char* loc3, char *loc4);
void release_detection();
void reinit_detection(char* cascade_location);
void release_detection_multiple() ;

CvSeq* mycvHaarDetectObjects( const CvArr* _img,
							 CvHaarClassifierCascade* cascade,
							 CvMemStorage* storage, double scale_factor,
							 int min_neighbors, int flags, CvSize min_size );
void detect_multiple( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
					 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
					 CvSize sz);
CGImageRef UIGetScreenImage();


#endif

enum{FACE=0, TEST1, NOSE, MOUTH, EYE, LEYE, REYE, BG};
#define FREE_MEMORY [UIDevice currentDevice].availableMemory

#define OPAQUE_HEXCOLOR(c) [UIColor colorWithRed:((c>>16)&0xFF)/255.0 \
green:((c>>8)&0xFF)/255.0 \
blue:(c&0xFF)/255.0 \
alpha:1.0]



static CvMemStorage *storage = 0;
camshift *cs;

#ifdef NOT_USED
@interface MyBayesClassifierWrapper : NSObject
{
@public
	struct CvNormalBayesClassifier *bayes;
};

@property (nonatomic, assign) struct CvNormalBayesClassifier *bayes; 

//+ (MyBayesClassifierWrapper *)instance;
-(id)init;
+(void)train:(CvMat *)t r:(CvMat *)r;
+(void)predict:(CvMat *)t r:(CvMat *)r;
@end

@implementation MyBayesClassifierWrapper
@synthesize bayes;
//static MyBayesClassifierWrapper *mybayes = nil;
//+ (MyBayesClassifierWrapper *)instance {
//	@synchronized(self) {
//        if (mybayes == nil) {
//            mybayes = [[self alloc] init];
//		}
//	}
//	return mybayes;
//}
-(id)init {
	if(self = [super init]) {
		bayes = new CvNormalBayesClassifier();
	}
	return self;
}
+(void)train:(CvMat *)t r:(CvMat *)r {
	bayes->train(t, r);
}
+(void)predict:(CvMat *)t r:(CvMat *)r {
	bayes->predict(t, r);
}

@end
#endif

@implementation CameraController

@synthesize faceTimer;
@synthesize previewTimer;
@synthesize model, nose_model, mouth_model, leye_model, reye_model;
//@synthesize plane, cylinder, cone;
//@synthesize nose;
@synthesize face_model_file, nose_model_file, mouth_model_file, leye_model_file, reye_model_file;
@synthesize glView;

static CameraController *instance = nil;
char *chars1 =nil, *chars2=nil, *chars3=nil, *chars4=nil, *chars5=nil;
extern CvHaarClassifierCascade* face_cascade;

//MyBayesClassifierWrapper *bayes;

CvRect *r[MAX_RECT];
CvRect *rNose[MAX_RECT];
CvRect *rMouth[MAX_RECT];
CvRect *rEyes[MAX_RECT];

CvRect *unscaled_r[MAX_RECT];
CvRect *unscaled_rNose[MAX_RECT];
CvRect *unscaled_rMouth[MAX_RECT];
CvRect *unscaled_rEyes[MAX_RECT];

+ (CameraController *)instance {
	@synchronized(self) {
        if (instance == nil) {
            instance = [[self alloc] init];
            //self.delegate=self;
            [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];  
            [[NSNotificationCenter defaultCenter] addObserver:instance
                                                     selector:@selector(deviceOrientationDidChange:)
                                                         name:@"UIDeviceOrientationDidChangeNotification"
                                                       object:nil]; 
			
			chars1 = (char*)malloc(512);
			chars2 = (char*)malloc(512);
			chars3 = (char*)malloc(1024);
			chars4 = (char*)malloc(512);
			chars5 = (char*)malloc(512);
			
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
			
		}
    }
    return instance;
}

-(void)setFaceTimer:(NSTimer *)timer {
	if (faceTimer != timer) {
		[faceTimer invalidate];
		[faceTimer release];
		faceTimer = [timer retain];
	}
}

-(void)setPreviewTimer:(NSTimer *)timer {
    if (previewTimer != timer) {
        [previewTimer invalidate];
        [previewTimer release];
        previewTimer = [timer retain];
    }
}

- (void)deviceOrientationDidChange:(id)ignore {
    UIDevice *device = [UIDevice currentDevice];
    o = device.orientation;
}

-(NSString *)stringPad:(int)numPad {
	NSMutableString *pad = [NSMutableString stringWithCapacity:1024];
	for (int i=0; i<numPad; i++) {
		[pad appendString:@"  "];
	}
	return pad; 
}

-(void)inspectView: (UIView *)theView depth:(int)depth path:(NSString *)path {
	
	if (depth==0) {
		NSLog(@"-------------------- <view hierarchy> -------------------");
	}
	
	NSString *pad = [self stringPad:depth];
	
	// print some information about the current view
	//
	NSLog([NSString stringWithFormat:@"%@.description: %@",pad,[theView description]]);
	if ([theView isKindOfClass:[UIImageView class]]) {
		NSLog([NSString stringWithFormat:@"%@.class: UIImageView",pad]);
	} else if ([theView isKindOfClass:[UILabel class]]) {
		NSLog([NSString stringWithFormat:@"%@.class: UILabel",pad]);
		NSLog([NSString stringWithFormat:@"%@.text: ",pad,[(UILabel *)theView text]]);		
	} else if ([theView isKindOfClass:[UIButton class]]) {
		NSLog([NSString stringWithFormat:@"%@.class: UIButton",pad]);
		NSLog([NSString stringWithFormat:@"%@.title: ",pad,[(UIButton *)theView titleForState:UIControlStateNormal]]);		
	}
	NSLog([NSString stringWithFormat:@"%@.frame: %.0f, %.0f, %.0f, %.0f", pad, theView.frame.origin.x, theView.frame.origin.y,
		   theView.frame.size.width, theView.frame.size.height]);
	NSLog([NSString stringWithFormat:@"%@.subviews: %d",pad, [theView.subviews count]]);
	NSLog(@" ");
	
	// gotta love recursion: call this method for all subviews
	//
	for (int i=0; i<[theView.subviews count]; i++) {
		NSString *subPath = [NSString stringWithFormat:@"%@/%d",path,i];
		NSLog([NSString stringWithFormat:@"%@--subview-- %@",pad,subPath]);		
		[self inspectView:[theView.subviews objectAtIndex:i]  depth:depth+1 path:subPath];
	}
	
	if (depth==0) {
		NSLog(@"-------------------- </view hierarchy> -------------------");
	}
	
}

- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image {
	CGImageRef imageRef = image.CGImage;
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	IplImage *iplimage = cvCreateImage(cvSize(image.size.width, image.size.height), IPL_DEPTH_8U, 4);
	CGContextRef contextRef = CGBitmapContextCreate(iplimage->imageData, iplimage->width, iplimage->height,
													iplimage->depth, iplimage->widthStep,
													colorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault);
	CGContextDrawImage(contextRef, CGRectMake(0, 0, image.size.width, image.size.height), imageRef);
	CGContextRelease(contextRef);
	CGColorSpaceRelease(colorSpace);
	
	IplImage *ret = cvCreateImage(cvGetSize(iplimage), IPL_DEPTH_8U, 3);
	cvCvtColor(iplimage, ret, CV_RGBA2BGR);
	cvReleaseImage(&iplimage);
	
	return ret;
}

//static int init =0;
//// Global variables
IplImage  * pVideoFrameCopy = 0;


- (void)detectFaceThread {
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    [self retain];
	
	//double myscale=1.5;
	
	
#if !TARGET_IPHONE_SIMULATOR
#if 0
	if((fav.totFaces) && (fav.totEyes ==2) && (fav.totNoses ==1) && (fav.totMouths ==1)) {
		
#if 1	
		
		CGImageRef screen = UIGetScreenImage();
		UIImage *viewImage = [UIImage imageWithCGImage:screen];
		CGImageRelease(screen);
		static IplImage *gray, *small_img;
		
		IplImage *img = [self CreateIplImageFromUIImage:viewImage];	
		
		if(!gray) {
			gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
			small_img = cvCreateImage( cvSize( cvRound (img->width/myscale),
											  cvRound (img->height/myscale)), 8, 1 );
		}	
		cvCvtColor( img, gray, CV_BGR2GRAY );
		cvResize( gray, small_img, CV_INTER_LINEAR );
		cvEqualizeHist( small_img, small_img );
		cvClearMemStorage( storage );
		
		CvSeq* face = cvHaarDetectObjects(small_img, face_cascade, storage,
										  1.1, 2, 0, 
										  // CV_HAAR_DO_CANNY_PRUNING,
										  cvSize(20, 20));
		
		
		
		
		if(face->total == 0) {
			cvReleaseImage(&img);
			[self performSelectorOnMainThread:@selector(reschedule) withObject:nil waitUntilDone:NO];
			[pool release];
			return;
		}
		//NSLog(@"1");
		CvRect* myr = (CvRect*)cvGetSeqElem( face, 0 );
		
#else
		CvRect * pFaceRect = r[0];
		// Initialize tracker
		if( !pVideoFrameCopy )
			pVideoFrameCopy = cvCreateImage( cvGetSize(snapshot), 8, 3 );
		cvCopy( snapshot, pVideoFrameCopy, 0 );
		pVideoFrameCopy->origin = snapshot->origin;
		
		if( 1 == pVideoFrameCopy->origin ) // 1 means the image is inverted
		{
			//NSLog(@"1a");
			cvFlip( pVideoFrameCopy, 0, 0 );
			pVideoFrameCopy->origin = 0;
		}
		
		if(!init) {
			
			init=1;
			if( !createTracker(cs, pVideoFrameCopy) ) return;
			
			// Set Camshift parameters
			setVmin(cs, 60);
			setSmin(cs, 50);
			
		}
		
		startTracking(cs, pVideoFrameCopy, pFaceRect);
		CvBox2D faceBox;
		// track the face in the new video frame
		faceBox = track(cs, pVideoFrameCopy);
		if( !pVideoFrameCopy->origin )
			faceBox.angle = -faceBox.angle;
		CvRect face_rect = cvRect( 
								  (int)(faceBox.center.x - faceBox.size.width/2), 
								  (int)(faceBox.center.y - faceBox.size.height/2),
								  (int)faceBox.size.width, (int)faceBox.size.height );
		
		
#endif	
		
		double myscaleW = (double)(myr->width)/(double)(unscaled_r[0]->width);
		myscaleW *= myscale;
		double myscaleH = (double)(myr->height)/(double)(unscaled_r[0]->height);
		myscaleH *= myscale;
		
		//CvRect nr, mr, er;
		
		r[0]->x = (int)((double)myr->x * myscale);
		r[0]->y = (int)((double)myr->y * myscale);
		r[0]->width = (int)((double)myr->width * myscale);
		r[0]->height = (int)((double)myr->height * myscale);
		
		
		fav.faces[0] = CGRectMake( r[0]->x, r[0]->y, r[0]->width, r[0]->height);
		
		for (int i=0; i<fav.totNoses; i++) {
			
			double x_coord = (((double)unscaled_rNose[i]->x)/((double)unscaled_r[0]->width) * (double)myr->width);
			double y_coord = (((double)unscaled_rNose[i]->y)/((double)unscaled_r[0]->height) * (double)myr->height);
			
			rNose[i]->x = (int)((x_coord + (double)(myr->x)) * myscale);
			rNose[i]->y = (int)((y_coord + (double)(myr->y)) * myscale);
			rNose[i]->width = (int)((double)(unscaled_rNose[i]->width) * myscaleW );
			rNose[i]->height = (int)((double)(unscaled_rNose[i]->height) * myscaleH); 
			
			fav.noses[i] = CGRectMake(rNose[i]->x, rNose[i]->y, rNose[i]->width, rNose[i]->height);
		}
		for (int i=0; i<fav.totMouths; i++) {
			
			
			double x_coord = (((double)unscaled_rMouth[i]->x)/((double)unscaled_r[0]->width) * (double)myr->width);
			double y_coord = (((double)unscaled_rMouth[i]->y)/((double)unscaled_r[0]->height) * (double)myr->height);
			
			rMouth[i]->x = (int)((x_coord + (double)(myr->x)) * myscale);
			rMouth[i]->y = (int)((y_coord + (double)(myr->y)) * myscale);
			
			rMouth[i]->width = (int)((double)(unscaled_rMouth[i]->width) * myscaleW );
			rMouth[i]->height = (int)((double)(unscaled_rMouth[i]->height) * myscaleH); 
			
			fav.mouth[i] = CGRectMake(rMouth[i]->x, rMouth[i]->y, rMouth[i]->width, rMouth[i]->height);
		}
		
		for (int i=0; i<fav.totEyes; i++) {
			
			double x_coord = (((double)unscaled_rEyes[i]->x)/((double)unscaled_r[0]->width) * (double)myr->width);
			double y_coord = (((double)unscaled_rEyes[i]->y)/((double)unscaled_r[0]->height) * (double)myr->height);
			
			rEyes[i]->x = (int)((x_coord + (double)(myr->x)) * myscale);
			rEyes[i]->y = (int)((y_coord + (double)(myr->y)) * myscale);
			
			rEyes[i]->width = (int)((double)(unscaled_rEyes[i]->width) * myscaleW );
			rEyes[i]->height = (int)((double)(unscaled_rEyes[i]->height) * myscaleH); 
			
			fav.eyes[i] = CGRectMake(rEyes[i]->x, rEyes[i]->y, rEyes[i]->width, rEyes[i]->height);
		}
		
		[fav setNeedsDisplay];
		cvReleaseImage(&img);
		[self performSelectorOnMainThread:@selector(reschedule) withObject:nil waitUntilDone:NO];
		[pool release];
		
		return;
	}
#endif	
	CGImageRef screen = UIGetScreenImage();
    UIImage *viewImage = [UIImage imageWithCGImage:screen];
    CGImageRelease(screen);
	
	IplImage *snapshot = [self CreateIplImageFromUIImage:viewImage];
	
	
	if(!snapshot) return;
	
	fav.totFaces=0; fav.totNoses=0; fav.totMouths=0; fav.totEyes=0;
	int tot[4]={fav.totFaces, fav.totNoses, fav.totMouths, fav.totEyes};
	
	//[glView drawView];
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
	
	detect_multiple(snapshot, r, rNose, rMouth, rEyes, tot, unscaled_r, unscaled_rNose,
								 unscaled_rMouth, unscaled_rEyes, cvSize(20,20));
	fav.totFaces = tot[0];
	int i=0;
	
	if(fav.totFaces) {	
		
		fav.faces[i] = CGRectMake(r[i]->x, r[i]->y, r[i]->width, r[i]->height);
		
		if(tot[1] > 0) fav.totNoses=tot[1];
		for (i=0; i<tot[1]; i++) 
			fav.noses[i] = CGRectMake(rNose[i]->x, rNose[i]->y, rNose[i]->width, rNose[i]->height);
		
		if(tot[2] > 0) fav.totMouths=tot[2];
		for (i=0; i<tot[2]; i++) 
			fav.mouth[i] = CGRectMake(rMouth[i]->x, rMouth[i]->y, rMouth[i]->width, rMouth[i]->height);
		
		if(tot[3] == 2) {
			fav.totEyes=tot[3];
			for (i=0; i<tot[3]; i++) 
				fav.eyes[i] = CGRectMake(rEyes[i]->x, rEyes[i]->y, rEyes[i]->width, rEyes[i]->height);
		}
		
		//[self myCompareImages: (IplImage *)snapshot];
		[fav setNeedsDisplay];
		//[glView drawView];
		[self setupPos:glView];
	}
	
	
	cvReleaseImage(&snapshot);
	
	[self performSelectorOnMainThread:@selector(reschedule) withObject:nil waitUntilDone:NO];
	[pool release];
#endif
	
}


//-(void)vibrate {
//    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
//}

-(void)reschedule {
	//  if(!isPreview && isDisplayed) {
	self.faceTimer = [NSTimer scheduledTimerWithTimeInterval:.10 
													  target:self 
													selector:@selector(detectFace) 
													userInfo:nil repeats:YES];    
	//}
}

-(void)detectFace {
    self.faceTimer = nil;
    [self performSelectorInBackground:@selector(detectFaceThread) withObject:nil];    
}

-(void)previewCheck {
    
    UIView *preview = [[[[[[[[[[self.view subviews] objectAtIndex:0]
							  subviews] objectAtIndex: 0]
							subviews] objectAtIndex: 0]
						  subviews] objectAtIndex: 0]
                        subviews] objectAtIndex: 2];
    
    // preview view path is /0/0/0/2, by default without a subview
    // if it has a subview, preview image is showing, so fix transform
    //
    if ([preview.subviews count] != 0 && !isPreview) {
        isPreview = YES;
    }
}

#ifdef NOT_USED

- (void)drawView1:(GLView*)view
{
	static GLfloat rotation = 0.0;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); 
	glColor4f(0.0, 0.5, 1.0, 1.0);
	// The coordinates of the rectangle are myRect.x,
	// myRect.y, myRect.width, myRect.height
	// Do I need a transform matrix here?
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, -1.0f, 1.0f);
	if(fav.totNoses)
		[plane drawSelf];
	
	glColor4f(1.0, 0.6, 0.3, 1.0);
	if(fav.totMouths)
		[mouth drawSelf];
	//[cylinder drawSelf];
	
	glColor4f(0.3, 1.0, 0.5, 1.0);
	if(fav.totEyes)
		[cone drawSelf];
	
	static NSTimeInterval lastDrawTime;
	if (lastDrawTime)
	{
		NSTimeInterval timeSinceLastDraw = [NSDate timeIntervalSinceReferenceDate] - lastDrawTime;
		rotation+=50 * timeSinceLastDraw;				
		Rotation3D rot;
		rot.x = rotation;
		rot.y = rotation;
		rot.z = rotation;
		//plane.currentRotation = rot;
		//cylinder.currentRotation = rot;
		//cone.currentRotation = rot;
	}
	lastDrawTime = [NSDate timeIntervalSinceReferenceDate];
}

#endif

- (void)drawView:(GLView*)view
{
	static GLfloat rotation = 0.0;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); 
	glColor4f(0.0, 0.5, 1.0, 1.0);
	// The coordinates of the rectangle are myRect.x,
	// myRect.y, myRect.width, myRect.height
	// Do I need a transform matrix here?
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, -1.0f, 1.0f);
	
	
	glColor4f(1.0, 0.6, 0.3, 1.0);
		
	static NSTimeInterval lastDrawTime;
	if (lastDrawTime)
	{
		NSTimeInterval timeSinceLastDraw = [NSDate timeIntervalSinceReferenceDate] - lastDrawTime;
		rotation+=50 * timeSinceLastDraw;				
		Rotation3D rot;
		rot.x = rotation;
		rot.y = rotation;
		rot.z = rotation;
		//plane.currentRotation = rot;
		//cylinder.currentRotation = rot;
		//cone.currentRotation = rot;
	}
	lastDrawTime = [NSDate timeIntervalSinceReferenceDate];
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
#if 1
	if(fav.totNoses) {
		
		
		//x = view.frame.size.width - rNose[0]->x - rNose[0]->width;
		y = view.frame.size.height - rNose[0]->y - rNose[0]->height;
		
		NSLog(@"nose-d0:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			 x,y,(float)(rNose[0]->width),(float)(rNose[0]->height));
		
		x = rNose[0]->x;
		idx=mynose;
		z = (float)(rNose[0]->width);
		w = (float)(rNose[0]->height);
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
		
		NSLog(@"eyes-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			  (float)(rEyes[0]->x),(float)(rEyes[0]->y),(float)(rEyes[0]->width),(float)(rEyes[0]->height));

		//x = view.frame.size.width - rEyes[0]->x - rEyes[0]->width;
		y = view.frame.size.height - rEyes[0]->y - rEyes[0]->height;
		x = rEyes[0]->x;
		
		NSLog(@"eyes0-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			  x,y,(float)(rEyes[0]->width),(float)(rEyes[0]->height));
		
		idx=myleye;
		z = (float)(rEyes[0]->width);
		w = (float)(rEyes[0]->height);
		[view SetupScreenPos:idx x:x y:y z:z w:w];
		
		//x = view.frame.size.width - rEyes[1]->x - rEyes[1]->width;
		y = view.frame.size.height - rEyes[1]->y - rEyes[1]->height;
		x = rEyes[1]->x;
		NSLog(@"eyes1-d:%.0f %.0f %.0f %.0f %.0f %.0f",view.frame.size.width, view.frame.size.height,
			  x,y,(float)(rEyes[1]->width),(float)(rEyes[1]->height));

		idx=myreye;
		z = (float)(rEyes[1]->width);
		w = (float)(rEyes[1]->height);
		[view SetupScreenPos:idx x:x y:y z:z w:w];
	}
	
#endif	
	
}

-(void)setupView:(GLView*)view
{		
	const GLfloat			lightAmbient[] = {0.2, 0.2, 0.2, 1.0};
	const GLfloat			lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
	
	const GLfloat			lightPosition[] = {5.0, 5.0, 15.0, 0.0}; 
	const GLfloat			light2Position[] = {-5.0, -5.0, 15.0, 0.0};
	const GLfloat			lightShininess = 0.0;
	const GLfloat			zNear = 0.01, zFar = 1000.0, fieldOfView = 45.0; 
	float zoom=1.0f;
	GLfloat size; 
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	
	size = zNear * tanf(DEGREES_TO_RADIANS(fieldOfView) / 2.0); 
	CGRect rect = view.bounds; 
	//glOrthof(-160.0f * zoom, 160.0f * zoom, -240.0f * zoom, 240.0f * zoom, 0.01f, 480.0f);
	glFrustumf(-size, size, -size / (rect.size.width / rect.size.height), size / 
			   (rect.size.width / rect.size.height), zNear, zFar); 
	//glEnable(GL_DEPTH_TEST);
	// glDepthMask(GL_TRUE);
	glViewport(0, 0, rect.size.width, rect.size.height);  
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 
	//glLightfv(GL_LIGHT0, GL_SHININESS, &lightShininess);
	
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light2Position); 
	//glLightfv(GL_LIGHT1, GL_SHININESS, &lightShininess);
	
	glLoadIdentity(); 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 	
	
	
	glGetError(); // Clear error codes
#if 0	
	NSString *path;
	OpenGLWaveFrontObject *theObject ;
	Vertex3D position;
	
	path = [[NSBundle mainBundle] pathForResource:@"myn5" ofType:@"obj"];
	theObject = [[OpenGLWaveFrontObject alloc] initWithPath:path];
	position.z = 1.0;
	position.y = 10.0;
	position.x = 10.0;
	
	//position.x = glView.frame.size.width - position.x ;
	//position.y = glView.frame.size.height - position.y ;
	
	//position.x -=  (glView.frame.size.width/2);
	//position.y -=  (glView.frame.size.height/2) ;
	
	
	theObject.currentPosition = position;
	self.nose = theObject;
	[theObject release];
	
	
	NSLog(@"nose-d:%.0f %.0f %.0f %.0f %.0f",glView.frame.size.width, glView.frame.size.height,
		  position.x,position.y,position.z);
	fav.totNoses=1;
	//path = [[NSBundle mainBundle] pathForResource:@"myn1" ofType:@"obj"];
	//theObject = [[OpenGLWaveFrontObject alloc] initWithPath:path];
	//position.z = -8.0;
	//position.y = 13.0;
	//position.x = 2.0;
	//theObject.currentPosition = position;
	//self.nose1 = theObject;
	//[theObject release];
#endif	
	
}


-(void)setupView1:(GLView*)view
{
	const GLfloat zNear = 0.01, zFar = 1000.0, fieldOfView = 45.0; 
	GLfloat size; 
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION); 
	//glMatrixMode(GL_MODELVIEW); 
	size = zNear * tanf(MY_DEGREES_TO_RADIANS(fieldOfView) / 2.0); 
	CGRect rect = view.bounds; 
	glFrustumf(-size, size, -size / (rect.size.width / rect.size.height), size / 
			   (rect.size.width / rect.size.height), zNear, zFar); 
	NSLog(@"frustumd:%.8f %.8f %.0f %.0f",size, (size / (rect.size.width / rect.size.height)),
		  rect.size.width, rect.size.height  );
	glViewport(0, 0, rect.size.width, rect.size.height);  
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity(); 
	// +++lal
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	
	//NSString *path = [[NSBundle mainBundle] pathForResource:@"plane" ofType:@"obj"];
	NSString *path = [[NSBundle mainBundle] pathForResource:@"nose1" ofType:@"obj"];
	OpenGLWaveFrontObject *theObject = [[OpenGLWaveFrontObject alloc] initWithPath:path];
	Vertex3D position;
	position.z = -8.0;
	position.y = 3.0;
	position.x = 0.0;
	if(fav.totNoses) {
		
		position.y = rNose[0]->y ;
		position.x = rNose[0]->x ;
		
		position.x = glView.frame.size.width - position.x - rNose[0]->width;
		position.y = glView.frame.size.height - position.y - rNose[0]->height;
		
		//position.x -=  (glView.frame.size.width/2);
		//position.y -=  (glView.frame.size.height/2) ;
	}
	NSLog(@"plane-d:%.0f %.0f %.0f %.0f %.0f",glView.frame.size.width, glView.frame.size.height,
		  position.x,position.y,position.z);
	
	theObject.currentPosition = position;
	//self.plane = theObject;
	[theObject release];
	
	
	//path = [[NSBundle mainBundle] pathForResource:@"Cylinder" ofType:@"obj"];
	path = [[NSBundle mainBundle] pathForResource:@"mouth1" ofType:@"obj"];
	theObject = [[OpenGLWaveFrontObject alloc] initWithPath:path];
	position.z = -8.0;
	
	position.y = 15.0;
	position.x = 7.5;
	
	//position.y = -3.0;
	//position.x = -1.5;
	if(fav.totMouths) {
		position.y = rMouth[0]->y;
		position.x = rMouth[0]->x;
		
		position.x = glView.frame.size.width - position.x - rMouth[0]->width;
		position.y = glView.frame.size.height - position.y - rMouth[0]->height;
		
		//position.x -=  (glView.frame.size.width/2);
		//position.y -=  (glView.frame.size.height/2) ;
	}
	NSLog(@"cyl-d:%.0f %.0f %.0f %.0f %.0f",glView.frame.size.width, glView.frame.size.height,
		  position.x,position.y,position.z);
	theObject.currentPosition = position;
	//self.cylinder = theObject;
	[theObject release];
	
	
	//path = [[NSBundle mainBundle] pathForResource:@"cone" ofType:@"obj"];
	path = [[NSBundle mainBundle] pathForResource:@"mouth2" ofType:@"obj"];
	theObject = [[OpenGLWaveFrontObject alloc] initWithPath:path];
	position.z = -8.0;
	
	//position.y = 8.0;
	//position.x = 5.5;
	
	//position.y = -1.0;
	//position.x = 1.0;
	position.y = 140;
	position.x = -70;
	if(fav.totEyes) {
		position.y = rEyes[0]->y;
		position.x = rEyes[0]->x;
		
		position.x = glView.frame.size.width - position.x - rEyes[0]->width;
		position.y = glView.frame.size.height - position.y - rEyes[0]->height;
		
		//position.x -=  (glView.frame.size.width/2);
		//position.y -=  (glView.frame.size.height/2) ;
	}
	NSLog(@"cone-d:%.0f %.0f %.0f %.0f %.0f",glView.frame.size.width, glView.frame.size.height,
		  position.x,position.y,position.z);
	
	theObject.currentPosition = position;
	//self.cone = theObject;
	[theObject release];
	
	
	
}

#ifdef NOT_USED

CvMat* response_data=NULL;
//CvNormalBayesClassifier *bayes=NULL;
#define RESIZED_WIDTH 15
#define RESIZED_HEIGHT 15
#define NCHAN 3
#define NDEPTH 8

#define NUM_NOSE_IMAGES 1 //25
#define NUM_MOUTH_IMAGES 1 //25
#define NUM_BG_IMAGES 1
#define NUM_FEATURES ((RESIZED_WIDTH * RESIZED_HEIGHT) * NCHAN)

//extern "C" void populateFeatureData(CvMat* train, CvMat* responses, int iFeatureType, int size );
unsigned int num_rows_in_train=0;
// Produces features for train/test data
- (void) populateFeatureData:(CvMat* )train responses:(CvMat *)responses iFeatureType:(int)iFeatureType size:(int)size  
{
	unsigned int z, i, j, h; 
	int response = iFeatureType;
	//unsigned char* hsv_values = NULL;
	unsigned char myFeature[128];
	unsigned char temp[128];
	
	int row_num=0; /* starts from 0 */
	row_num = num_rows_in_train;
	
	for (h = 0; h < (unsigned int)size; h++) { 
		
		int row = h + row_num;
		
		myFeature[0]='\0';
		temp[0]='\0';
		switch (iFeatureType) {
			case NOSE:
				strcpy((char *)myFeature, "nose");
				break;
			case MOUTH:
				strcpy((char *)myFeature, "mouth");
				break;
			case LEYE:
				strcpy((char *)myFeature, "eye");
				break;
			case REYE:
				strcpy((char *)myFeature, "reye");
				break;
			case BG:
				strcpy((char *)myFeature, "bg");
				break;
			default:
				break;
		}
		
		
		
		sprintf((char *)temp, "%d.jpg", (h+1));
		//sprintf((char *)temp, "%d", (h+1));
		strcat((char *)myFeature, (char *)temp);
		
		NSString* imageName = [[NSString alloc] initWithBytes:myFeature length:sizeof(myFeature) encoding:NSASCIIStringEncoding];
#if 0
		UIImage * currentImage = [UIImage imageNamed:imageName cache:NO];
#else
		//NSString *fileLocation = [[NSBundle mainBundle] pathForResource:imageName ofType:nil];
		//NSData *imageData = [NSData dataWithContentsOfFile:imageName];
		
		//UIImage * currentImage = [UIImage imageWithData:imageData];
		
		UIImage *currentImage = [[UIImage alloc] initWithContentsOfFile:[NSString stringWithFormat:@"%@/%@", [[NSBundle mainBundle] bundlePath], imageName]];
#endif
		//IplImage* original_img = cvLoadImage((char *)myFeature, 1);
		IplImage* original_img = [self CreateIplImageFromUIImage:currentImage];
		if (!original_img) { 
			//if(imageName) [imageName release];
			//if(currentImage) [currentImage release];
			return; 
		}
		IplImage* resized_img = cvCreateImage(cvSize(RESIZED_WIDTH,RESIZED_HEIGHT), NDEPTH, NCHAN);
		//  original_img->depth,original_img->nChannels);
		IplImage* gray_img = cvCreateImage(cvSize(original_img->width,original_img->height), NDEPTH, NCHAN);
		// original_img->depth,original_img->nChannels);
		
		if(NCHAN ==1) {
			cvCvtColor( original_img, gray_img, CV_BGR2GRAY );
			cvResize( gray_img, resized_img, CV_INTER_LINEAR );
			cvEqualizeHist( resized_img, resized_img );
		}
		else if(NCHAN==3) {
			
			cvResize( original_img, resized_img);
			
		}
		
		z=0;
		for (i = 0; i < resized_img->height; i++) {
			for (j = 0; j < resized_img->width; j++) { 
				
				//hsv_values = &CV_IMAGE_ELEM(hsv_img, uchar, i, j*3); 
				//if (train) cvmSet(train, row, z++, (double)hsv_values[2]);
				//if (train) cvmSet(train, h, z++, (double)hsv_values[2]); 
				//if (test) cvmSet(test, h, z++, (double)hsv_values[2]); 
				//hsv_values = NULL;
				
				if (train) {
					for(int k=0; k < NCHAN; k++) {
						unsigned char *pixel  = &CV_IMAGE_ELEM(resized_img, uchar, i, (j*3)+k);
						cvmSet(train, row, z++, (double)pixel[0]);
					}
				}
				
			}
		} 
		if (responses) cvmSet(responses, row, 0, (double)response); 
		num_rows_in_train++;
		NSLog(@"h=%d", h);
		cvReleaseImage(&gray_img); 
		
		cvReleaseImage(&original_img); 
		cvReleaseImage(&resized_img);
		//if(imageName) [imageName release];
		if(currentImage) [currentImage release];
		
	}
}


- (void) myInitNormalBayesClassifier
{
	int num_train_images = (NUM_NOSE_IMAGES + NUM_MOUTH_IMAGES + NUM_BG_IMAGES);
	int num_nose_images = NUM_NOSE_IMAGES;
	int num_mouth_images = NUM_MOUTH_IMAGES;
	int num_bg_images = NUM_BG_IMAGES;
	
	// create matrices for holding data
	CvMat* train_data = cvCreateMat(num_train_images, NUM_FEATURES, CV_32FC1); 
	response_data = cvCreateMat(num_train_images, 1, CV_32FC1);
	
	// populate train data and train classes(responses)
	int iFeature = NOSE;
	[self populateFeatureData: (CvMat *)train_data responses:(CvMat *)response_data 
				 iFeatureType:iFeature size:num_nose_images];
	iFeature = MOUTH;
	[self populateFeatureData: (CvMat *)train_data responses:(CvMat *)response_data 
				 iFeatureType:iFeature size:num_mouth_images];
	iFeature = BG;
	[self populateFeatureData: (CvMat *)train_data responses:(CvMat *)response_data 
				 iFeatureType:iFeature size:num_bg_images];
	// If you want to apply an other classifier, you just have to 
	// define the type of "bayes" differently 
	// ("bayes" is the name of the variable) 
	// CvSVM bayes;
	bayes->bayes->train(train_data, response_data);
	//[bayes train: (CvMat *)train_data r:(CvMat *)response_data];
	// cleanup steps
	cvReleaseMat(&train_data); 
	//cvReleaseMat(&response_data); 
	
	
	
}

- (void) myCompareImages: (IplImage *)img
{
	IplImage *gray, *gray1, *small_img;
    int i, j, k;
	
	CvMat *testN, *testM;
	if(fav.totNoses) testN = cvCreateMat(fav.totNoses, NUM_FEATURES, CV_32FC1);
	if(fav.totMouths) testM = cvCreateMat(fav.totMouths, NUM_FEATURES, CV_32FC1);
	
	
	for (i=0; i<fav.totNoses; i++) {
		
		CvRect *r = unscaled_rNose[i];
		r->x += unscaled_r[i]->x;
		r->y += unscaled_r[i]->y;
		
		//IplImage *gray1 = cvCreateImage( cvSize(img->width,img->height), img->depth, 3 );
		IplImage * img1 = cvCloneImage(img);
		//cvCvtColor( img1, gray1, CV_BGR2GRAY );
		cvSetImageROI(img1, *r);
		IplImage *img2 = cvCreateImage(cvGetSize(img1),
									   img1->depth,
									   img1->nChannels);
		cvCopy(img1, img2, NULL);
		
		IplImage *img3 = cvCreateImage( cvSize( RESIZED_WIDTH, RESIZED_HEIGHT), NDEPTH, NCHAN );
		cvResize(img2, img3);
		//cvEqualizeHist(img3, img3); // to crop out a new image of the give size
		cvResetImageROI(img1);
		int z=0;
		for (j = 0; j < img3->height; j++) {
			for (k = 0; k < img3->width; k++) { 
				
				for(int l=0; l < NCHAN; l++) {
					unsigned char *pixel  = &CV_IMAGE_ELEM(img3, uchar, j, (k*3)+l);
					if(testN) cvmSet(testN, i, z++, (double)pixel[0]);
				}
			}
		} 
		cvReleaseImage(&img2); 
		cvReleaseImage(&img3);
		//cvReleaseImage(&gray1);
		cvReleaseImage(&img1);
	}
	if(fav.totNoses) {
		CvMat* response = cvCreateMat(1, 1, CV_32FC1); 
		CvMat* temp = cvCreateMat(1, NUM_FEATURES, CV_32FC1);
		
		for (i=0; i<fav.totNoses; i++) {
			
			cvGetRow(testN, temp, i);
			bayes->bayes->predict(temp, response);
			double dRes = cvmGet(response, 0, 0);
			NSLog(@"n%d predict = %f", i, dRes);
		}
		cvReleaseMat(&testN);
		cvReleaseMat(&response);
		cvReleaseMat(&temp);
	}
	
	
	
	for (i=0; i<fav.totMouths; i++) {
		
		CvRect *r = unscaled_rMouth[i];
		r->x += unscaled_r[i]->x;
		r->y += unscaled_r[i]->y;
		//double scaleW = (double)(RESIZED_WIDTH)/(double)(img->width);
		//double scaleH = (double)(RESIZED_WIDTH)/(double)(img->height);
		
		//IplImage *gray1 = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
		IplImage * img1 = cvCloneImage(img);
		//cvCvtColor( img1, gray1, CV_BGR2GRAY );
		cvSetImageROI(img1, *r);
		IplImage *img2 = cvCreateImage(cvGetSize(img1),
									   img1->depth,
									   img1->nChannels);
		cvCopy(img1, img2, NULL);
		
		IplImage *img3 = cvCreateImage( cvSize( RESIZED_WIDTH, RESIZED_HEIGHT), NDEPTH, NCHAN );
		cvResize(img2, img3);
		//cvEqualizeHist(img3, img3); // to crop out a new image of the give size
		cvResetImageROI(img1);
		
		int z=0;
		for (j = 0; j < img3->height; j++) {
			for (k = 0; k < img3->width; k++) { 
				
				for(int l=0; l < NCHAN; l++) {
					unsigned char *pixel  = &CV_IMAGE_ELEM(img3, uchar, j, (k*3)+l);
					if(testM) cvmSet(testM, i, z++, (double)pixel[0]);
				}
				
			}
		} 
		cvReleaseImage(&img2);
		cvReleaseImage(&img3);
		//cvReleaseImage(&gray1);
		cvReleaseImage(&img1);
	}
	if(fav.totMouths) {
		CvMat* response = cvCreateMat(1, 1, CV_32FC1);
		CvMat* temp = cvCreateMat(1, NUM_FEATURES, CV_32FC1);
		
		for (i=0; i<fav.totMouths; i++) {
			
			cvGetRow(testM, temp, i);
			bayes->bayes->predict(temp, response);
			double dRes = cvmGet(response, 0, 0);
			NSLog(@"m%d predict = %f", i, dRes);
		}
		cvReleaseMat(&testM);
		cvReleaseMat(&response);
		cvReleaseMat(&temp);
	}
	
	return;
}
#endif

- (void)viewDidAppear: (BOOL)animated {
	[super viewDidAppear:animated];
    
	fav = [[FaceAugmentingView alloc] initWithFrame:self.view.frame];
	
	fav.backgroundColor = [UIColor clearColor];
	[self.view addSubview:fav];
	fav.totEyes=0;
	fav.totNoses=0;
	fav.totMouths=0;
	fav.totNoses=0;
	
	face_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_frontalface_alt2.xml" ofType:@"gz"];
	[face_model_file getCString:chars1 maxLength:512 encoding:NSUTF8StringEncoding];
	
	nose_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_nose.xml" ofType:@"gz"];
	[nose_model_file getCString:chars2 maxLength:512 encoding:NSUTF8StringEncoding];
	
	//mouth_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_mouth.xml" ofType:@"gz"];
	mouth_model_file = [[NSBundle mainBundle] pathForResource:@"Mouth.xml" ofType:@"gz"];
	[mouth_model_file getCString:chars3 maxLength:512 encoding:NSUTF8StringEncoding];
	
	leye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_lefteye.xml" ofType:@"gz"];
	[leye_model_file getCString:chars4 maxLength:512 encoding:NSUTF8StringEncoding];
	
	reye_model_file = [[NSBundle mainBundle] pathForResource:@"haarcascade_mcs_righteye.xml" ofType:@"gz"];
	[reye_model_file getCString:chars5 maxLength:512 encoding:NSUTF8StringEncoding];
	
	init_detection_multiple(chars1, chars2, chars3, chars4, chars5);
	
	cs = (camshift *)malloc(sizeof(camshift));
    isPreview = NO;
    isDisplayed = YES;
	
	glView = [[GLView alloc] initWithFrame:self.view.frame];
	[self.view addSubview:glView];
	
	glView.controller = self;
	//glView.animationInterval = 1.0 / kRenderingFrequency;
	//[glView startAnimation];
	//[glView release];
	
	//  bayes = [[MyBayesClassifierWrapper alloc] init];
	//[self myInitNormalBayesClassifier];
	
    self.faceTimer = [NSTimer scheduledTimerWithTimeInterval:.25 
													  target:self 
													selector:@selector(detectFace) 
													userInfo:nil repeats:YES];
	//bayes = new CvNormalBayesClassifier;
	
	storage = cvCreateMemStorage(0);
	//  self.previewTimer = [NSTimer scheduledTimerWithTimeInterval:.25
	//                                                        target:self
	//                                                     selector:@selector(previewCheck)
	//                                                     userInfo:nil repeats:YES];    
    
#if !TARGET_IPHONE_SIMULATOR
	
    [self inspectView:self.view depth:0 path:@""];
    
#if 0
    if(self.sourceType == UIImagePickerControllerSourceTypeCamera) {
        UIImageView *overlay = [[[[[[[[[[self.view subviews] objectAtIndex:0]
                                       subviews] objectAtIndex:0]
                                     subviews] objectAtIndex:0]
								   subviews] objectAtIndex:3]
								 subviews] objectAtIndex:0];  
        
        UILabel *label = [[[[[[[[[[self.view subviews] objectAtIndex:0]
								 subviews] objectAtIndex:0]
							   subviews] objectAtIndex:0]
							 subviews] objectAtIndex:3]
						   subviews] objectAtIndex:1];
		
        UIButton *button = [[[[[[[[[[[[self.view subviews] objectAtIndex:0]
                                     subviews] objectAtIndex:0]
                                   subviews] objectAtIndex:0]
                                 subviews] objectAtIndex:3]
                               subviews] objectAtIndex:2]
                             subviews] objectAtIndex:0];
        
        [button addTarget:self action:@selector(cancelButtonAction:) forControlEvents:UIControlEventTouchUpInside];
        
        UIButton *captureButton = [[[[[[[[[[[[self.view subviews] objectAtIndex:0]
											subviews] objectAtIndex:0]
										  subviews] objectAtIndex:0]
										subviews] objectAtIndex:3]
									  subviews] objectAtIndex:2]
									subviews] objectAtIndex:1];
        
        [captureButton addTarget:self action:@selector(captureButtonAction:) forControlEvents:UIControlEventTouchUpInside];        
		
        label.hidden = YES;
        overlay.hidden = YES;        
    }
#endif
#endif
	
}

- (void)captureButtonAction:(id)sender {
	// self.faceTimer = nil;
	
}

- (void)cancelButtonAction:(id)sender {
    
	if (isPreview) {
		//  isPreview = NO;
		// [self performSelectorOnMainThread:@selector(reschedule) withObject:nil waitUntilDone:YES];        
    } else {
		//  self.faceTimer = nil;
		//  [self dismissModalViewControllerAnimated:YES];        
    }
    
}

- (void)viewDidDisappear:(BOOL)animated {
    self.faceTimer = nil;
    self.previewTimer = nil;
    isDisplayed = NO;
	
	[super viewDidDisappear:animated];
}

-(void) dealloc {
    self.faceTimer = nil;
    self.previewTimer = nil;
    release_detection_multiple();
	if(chars1) free(chars1);
	if(chars2) free(chars2);
	if(chars3) free(chars3);
	if(chars4) free(chars4);
	if(chars5) free(chars5);
	
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
	//if(bayes) [bayes dealloc];
	cvReleaseMemStorage(&storage);
	//[plane release];
	//if(nose) [nose release];
	[glView release];
	[super dealloc];
}

@end
