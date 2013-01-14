//
//  GLView.h
//  Wavefront OBJ Loader
//

#import "Interfaces.hpp"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "ConstantsAndMacros.h"


@class iAngelEyesController;
@interface GLView : UIView
{
@private
	// The pixel dimensions of the backbuffer
	GLint backingWidth;
	GLint backingHeight;
	
	EAGLContext *context;
	GLuint viewRenderbuffer, viewFramebuffer;
	GLuint depthRenderbuffer;
	NSTimer *animationTimer;
	NSTimeInterval animationInterval;

	iAngelEyesController *controller;
	BOOL controllerSetup;
	BOOL draw;
	void *noseTextureData;
	
	BOOL m_noTimer;
    IApplicationEngine* m_applicationEngine;
    IRenderingEngine* m_renderingEngine;
    IResourceManager* m_resourceManager;
    EAGLContext* m_context;
    float m_timestamp;
	int m_textureChange;
	CGPoint m_textureLoc;
	
}

@property(nonatomic, retain) iAngelEyesController *controller;
@property(nonatomic, assign) BOOL draw;
@property(nonatomic, assign) void *noseTextureData;
@property(nonatomic, assign) float m_timestamp;
@property(nonatomic, assign) int m_textureChange;
@property(nonatomic, assign) CGPoint m_textureLoc;
@property NSTimeInterval animationInterval;

-(void)startAnimation;
-(void)stopAnimation;
-(void)drawView;
- (void) drawView: (CADisplayLink*) displayLink;
- (void) ClearBuffer;
- (CGPoint) GetTextureLoc;
-(void) SetupScreenPos:(int)idx x:(float)x y:(float)y z:(float)z w:(float)w;
-(void) GetSurfaceScreenPos:(int)idx x:(float *)x y:(float *)y z:(float *)z w:(float *)w;
-(void) MoveObject;
-(void) RotateObject;
-(void) ResizeObject;
-(void) BrowseObjects:(int)n;
-(void) setNoseTextureData:(void *)d w:(int)w h:(int)h;
-(void) setMatProperty:(int)p val:(float)value;
-(void) ResetMovement;
-(void) SetDraw:(BOOL)d;
-(void) SetAlphaCorrection: (int)val;
-(void) ToggleLight: (int)val;
-(void) PreserveOrientation: (int)val;
-(void) SetZoomFactor: (float)val;
- (UIImage*) getGLScreenshot;
@end
