//
//  GLView.m

#import <iostream>
#import <fstream>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "GLView.h"
#import "iAngelEyesController.h"

using namespace std;

#define GL_RENDERBUFFER 0x8d41
#if 0
@interface GLView (private)

- (id)initGLES;
- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

@end
#endif

@implementation GLView

@synthesize animationInterval;
@synthesize m_timestamp;
@synthesize draw;
@synthesize noseTextureData, m_textureChange, m_textureLoc;
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

//CAEAGLLayer* eaglLayer;
//CGRect myFrame;
//static int once=0;

- (id) initWithFrame: (CGRect) frame
{
    if (self = [super initWithFrame:frame])
    {
       CAEAGLLayer*  eaglLayer = (CAEAGLLayer*) self.layer;
        //eaglLayer.opaque = YES;
		// +++lal
		//myFrame=frame;
		eaglLayer.opaque = NO;
#if 0		
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_context = [[EAGLContext alloc] initWithAPI:api];
        
        if (!m_context) {
            api = kEAGLRenderingAPIOpenGLES1;
            m_context = [[EAGLContext alloc] initWithAPI:api];
        }
		if (!m_context || ![EAGLContext setCurrentContext:m_context]) {
            [self release];
            return nil;
        }

		self.multipleTouchEnabled=YES;
        m_resourceManager = Darwin::CreateResourceManager();
		
        if (api == kEAGLRenderingAPIOpenGLES1) {
            NSLog(@"Using OpenGL ES 1.1");
            m_renderingEngine = SolidES1::CreateRenderingEngine(m_resourceManager);
        } else {
            NSLog(@"Using OpenGL ES 2.0");
            m_renderingEngine = SolidES2::CreateRenderingEngine(m_resourceManager);
        }
#else
		EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES1;
        m_context = [[EAGLContext alloc] initWithAPI:api];
		if (!m_context || ![EAGLContext setCurrentContext:m_context]) {
            [self release];
            return nil;
        }
		
		self.multipleTouchEnabled=YES;
        m_resourceManager = Darwin::CreateResourceManager();
		
		//NSLog(@"Using OpenGL ES 1.1");
		m_renderingEngine = SolidES1::CreateRenderingEngine(m_resourceManager);
        
#endif
		m_applicationEngine = ObjViewer::CreateApplicationEngine(m_renderingEngine, m_resourceManager);
		m_noTimer=NO;
		
		
		 [m_context
		 // renderbufferStorage:GL_RENDERBUFFER
		 renderbufferStorage:GL_RENDERBUFFER_OES
		fromDrawable:(id<EAGLDrawable>)eaglLayer];

		int width = CGRectGetWidth(frame);
        int height = CGRectGetHeight(frame);
        m_applicationEngine->Initialize(width, height);
               
        [self drawView: nil];
        m_timestamp = CACurrentMediaTime();
  

        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self
												  selector:@selector(drawView:)];
        
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop]
		//[displayLink addToRunLoop:[NSRunLoop mainRunLoop]
						  forMode:NSDefaultRunLoopMode];

    }
    return self;
}

- (void) drawView
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
    m_applicationEngine->Render();
	
	if(m_applicationEngine->PresentBuffer()) [m_context presentRenderbuffer:GL_RENDERBUFFER_OES];
	//if(m_applicationEngine->PresentBuffer()) [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void) drawView: (CADisplayLink*) displayLink
{
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
        m_applicationEngine->UpdateAnimation(elapsedSeconds);
    }
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
    if(draw) 
	{
		m_applicationEngine->Render();
	
		if(m_applicationEngine->PresentBuffer()) [m_context presentRenderbuffer:GL_RENDERBUFFER_OES];
		//if(m_applicationEngine->PresentBuffer()) [m_context presentRenderbuffer:GL_RENDERBUFFER];
	}
}

- (void) ClearBuffer
{
	m_renderingEngine->ClearBuffer();
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch, *touch1;
	CGPoint location, location1;
	
	if([[event allTouches] count] == 1) {
		
		touch=[[[event allTouches] allObjects] objectAtIndex:0];
		location=[touch locationInView:self];
		
		if(m_textureChange) {
			m_textureLoc.x = location.x;
			m_textureLoc.y = location.y;
			m_textureChange = 0;
			[controller setTexture:self];
			
		}
		else {
			m_applicationEngine->OnFingerDown(vec2(location.x, location.y));
		}
		//cout << " 1.1 " << endl;
			
	}
	
	else if([[event allTouches] count] == 2) {
		
		touch=[[[event allTouches] allObjects] objectAtIndex:0];
		location=[touch locationInView:self];
		
		touch1=[[[event allTouches] allObjects] objectAtIndex:1];
		location1=[touch1 locationInView:self];
		m_applicationEngine->OnTwoFingerDown(vec2(location.x, location.y), 
											 vec2(location1.x, location1.y));
		//cout << " 1.2 " << endl;
	}
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch, *touch1;
	CGPoint location, location1;
	
	if([[event allTouches] count]==1) {
		
		touch = [touches anyObject];
		location  = [touch locationInView: self];
		m_applicationEngine->OnFingerUp(vec2(location.x, location.y));
		
	}
	else if([[event allTouches] count]==2){
		
		touch=[[[event allTouches] allObjects] objectAtIndex:0];
		location=[touch locationInView:self];
		
		touch1=[[[event allTouches] allObjects] objectAtIndex:1];
		location1=[touch1 locationInView:self];
		
		m_applicationEngine->OnTwoFingerUp(vec2(location.x, location.y), vec2(location1.x, location1.y));
	}
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch1, *touch2;
	CGPoint peviousFinger1, currentFinger1;
	CGPoint currentFinger2;
	
	if([[event allTouches] count] == 1) {
		
		touch1 = [touches anyObject];
		peviousFinger1  = [touch1 previousLocationInView: self];
		currentFinger1 = [touch1 locationInView: self];
		m_applicationEngine->OnFingerMove(vec2(peviousFinger1.x, peviousFinger1.y),
										  vec2(currentFinger1.x, currentFinger1.y));
	}
	else if([[event allTouches] count] == 2) {
		
		touch1 = [[[event allTouches] allObjects] objectAtIndex:0];
		currentFinger1 = [touch1 locationInView:self];
		
		touch2 = [[[event allTouches] allObjects] objectAtIndex:1];
		currentFinger2 = [touch2 locationInView:self];

		m_applicationEngine->OnTwoFingerMove(
											 vec2(currentFinger1.x, currentFinger1.y),
											 
											 vec2(currentFinger2.x, currentFinger2.y));
		
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self touchesBegan:touches withEvent:event];
}


- (void)startAnimation
{
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

- (void)stopAnimation
{
	[animationTimer invalidate];
	animationTimer = nil;
}

- (void)setAnimationInterval:(NSTimeInterval)interval
{
	animationInterval = interval;
	
	if(animationTimer)
	{
		[self stopAnimation];
		[self startAnimation];
	}
}

-(iAngelEyesController *)controller
{
	return controller;
}

-(void)setController:(iAngelEyesController *)d
{
	controller = d;
	//controllerSetup = ![controller respondsToSelector:@selector(setupView:)];
	//controllerSetup = ![controller respondsToSelector:@selector(setTexture:)];
}

- (CGPoint) GetTextureLoc
{
	return m_textureLoc;
}
-(void) SetupScreenPos:(int)idx x:(float)x y:(float)y z:(float)z w:(float)w
{
	m_applicationEngine->SetupScreenPos(idx, x, y, z, w);
}

-(void) GetSurfaceScreenPos:(int)idx x:(float *)x y:(float *)y z:(float *)z w:(float *)w
{
	m_applicationEngine->GetSurfaceScreenPos(idx, x, y, z, w);
}

-(void) MoveObject
{
	m_applicationEngine->MoveObject();
}

-(void) RotateObject
{
	m_applicationEngine->RotateObject();
}

-(void) ResizeObject
{
	m_applicationEngine->ResizeObject();
}

-(void) BrowseObjects:(int)n
{
	m_applicationEngine->BrowseObjects(n);
}

-(void) setNoseTextureData:(void *)data w:(int)w h:(int)h
{
	noseTextureData = data;
	m_renderingEngine->SetNoseTextureData(data, w, h);
}

-(void) setMatProperty:(int)p val:(float)value
{
	m_applicationEngine->ResetMovement();
	if(p != TEXTURE) {
		m_renderingEngine->SetMatProperty(p, value);
	}
	else {
		m_textureChange = 1;
	}
}

-(void) ResetMovement
{
	m_applicationEngine->ResetMovement();
}

-(void) SetDraw:(BOOL)d
{
	draw = d;
}

-(void) SetAlphaCorrection: (int)val
{
	m_renderingEngine->SetAlphaCorrection(val);
}

- (void)ToggleLight:(int)val 
{
	m_renderingEngine->ToggleLight(val);
}

- (void)PreserveOrientation:(int)val 
{
	
	m_applicationEngine->PreserveOrientation(val);
}

-(void) SetZoomFactor: (float)val
{
	m_applicationEngine->SetZoomFactor(val);
}

- (UIImage*) getGLScreenshot {
	int adjustedHt = (480 -53);
    NSInteger myDataLength = 320 * adjustedHt * 4;
	
    // allocate array and read pixels into it.
    GLubyte *buffer = (GLubyte *) malloc(myDataLength);
    glReadPixels(0, 0, 320, adjustedHt, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	
    // gl renders "upside down" so swap top to bottom into new array.
    // there's gotta be a better way, but this works.
    GLubyte *buffer2 = (GLubyte *) malloc(myDataLength);
    for(int y = 0; y <adjustedHt; y++)
    {
        for(int x = 0; x <320 * 4; x++)
        {
            buffer2[((adjustedHt -1) - y) * 320 * 4 + x] = buffer[y * 4 * 320 + x];
        }
    }
	
    // make data provider with data.
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer2, myDataLength, NULL);
	
    // prep the ingredients
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    int bytesPerRow = 4 * 320;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
    // make the cgimage
    CGImageRef imageRef = CGImageCreate(320, adjustedHt, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
	
    // then make the uiimage from that
    UIImage *myImage = [UIImage imageWithCGImage:imageRef];
    return myImage;
}

- (void)saveGLScreenshotToPhotosAlbum {
	UIImageWriteToSavedPhotosAlbum([self getGLScreenshot], nil, nil, nil);	
}

// If our view is resized, we'll be asked to layout subviews.
// This is the perfect opportunity to also update the framebuffer so that it is
// the same size as our display area.
-(void)layoutSubviews
{
#if 0
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
#endif
}

@end

namespace FacetedES2 { IRenderingEngine* CreateRenderingEngine() { return 0; } }
namespace SolidGL2 { IRenderingEngine* CreateRenderingEngine() { return 0; } }
namespace TexturedGL2 { IRenderingEngine* CreateRenderingEngine() { return 0; } }



