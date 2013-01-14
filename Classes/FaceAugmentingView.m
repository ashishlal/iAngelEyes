//
//  FaceAugmentingView.m
//  FaceDetector1
//

#import "FaceAugmentingView.h"


@implementation FaceAugmentingView
//@synthesize faces, noses, eyes, mouth;
@synthesize whatToPut, totFaces, totNoses, totEyes, totMouths;

- (CGRect *)faces
{
    return _faces;
}

- (CGRect *)noses
{
    return _noses;
}

- (CGRect *)eyes
{
    return _eyes;
}

- (CGRect *)mouth
{
    return _mouth;
}


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        ;
    }
	//totFaces =0;
	//totEyes =0;
	//totMouths=0;
	//totNoses=0;
	//showRect = FALSE;
    return self;
}

-(void)toggleShowRect:(int)val { 
	if(val) showRect = TRUE;
	else showRect = FALSE;
}

- (void)drawRect:(CGRect)rect {


	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextTranslateCTM (ctx, self.frame.size.width, self.frame.size.height);
	CGContextRotateCTM (ctx, M_PI);
	//NSLog(@"f:%d, n:%d m:%d e:%d",totFaces, totNoses, totMouths, totEyes);

//	if(1) {
	if(showRect) {
		//CGRect face = _faces[0];
		if(totFaces > 0) {
			[self drawFace:ctx];
		}

		
		if(totEyes > 0) {
			[self drawEyes:ctx];
		}
#if 0
		if(totNoses > 0) {
			[self drawNose:ctx];
		}
			
		if(totMouths > 0) {
			[self drawMouth:ctx];
		}
#endif
	}
	return;
}

-(void) drawFace:(CGContextRef)ctx {
	
	int i=0;
	//int scale=1;
	
	for(i=0; i < totFaces; i++) {
		
		CGRect r = _faces[i];
		r.origin.x = self.frame.size.width - r.origin.x - r.size.width;
		r.origin.y = self.frame.size.height - r.origin.y - r.size.height;
		
		CGRect todrawFace = CGRectMake(r.origin.x , r.origin.y , 
									   r.size.width , r.size.height );
		
		
		//NSLog(@"fd:%.0f %.0f %.0f %.0f %.0f %.0f",self.frame.size.width, self.frame.size.height,
		//	  r.origin.x,r.origin.y,r.size.width,r.size.height);
		[[UIColor yellowColor] set];
		CGContextStrokeRect(ctx, todrawFace);
	}
}


-(void) drawNose:(CGContextRef)ctx {
	int i=0;
	//int scale=1;

	for(i=0; i < totNoses; i++) {
		
		CGRect r = _noses[i];
		//NSLog(@"nd-1:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);
		r.origin.x = self.frame.size.width - r.origin.x - r.size.width;
		r.origin.y = self.frame.size.height - r.origin.y - r.size.height;
		
		CGRect todrawNose = CGRectMake(r.origin.x , r.origin.y , 
									   r.size.width , r.size.height);
		
		//NSLog(@"nd:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);
		[[UIColor redColor] set];
		CGContextStrokeRect(ctx, todrawNose);
	}
	
}

- (void) drawMouth:(CGContextRef)ctx {
	
	int i=0;
	
	for(i=0; i < totMouths; i++) {
		
		CGRect r = _mouth[i];
		
		NSLog(@"md-1:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);
		

		r.origin.x = self.frame.size.width - r.origin.x - r.size.width;
		r.origin.y = self.frame.size.height - r.origin.y - r.size.height;
		
		CGRect todrawMouth = CGRectMake(r.origin.x, r.origin.y , r.size.width, r.size.height);
		
		NSLog(@"md:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);
		[[UIColor blueColor] set];
//#if DEBUG_FALSE_POSITIVES
#if 1
		if(i==1) [[UIColor blackColor] set];
		if(i==2) [[UIColor orangeColor] set];
		if(i==3) [[UIColor magentaColor] set];
		NSLog(@"md:%.0f %.0f",self.frame.size.width,self.frame.size.height);
		NSLog(@"md:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);
#endif
		CGContextStrokeRect(ctx, todrawMouth);
		
	}
}

- (void) drawEyes:(CGContextRef)ctx {
	
	int i=0;

	
	for(i=0; i < totEyes; i++) {
		
		CGRect r = _eyes[i];
		r.origin.x = self.frame.size.width - r.origin.x - r.size.width;
		r.origin.y = self.frame.size.height - r.origin.y - r.size.height;
		
		CGRect todrawEyes = CGRectMake(r.origin.x , r.origin.y , r.size.width , r.size.height );

		[[UIColor greenColor] set];
#if DEBUG_FALSE_POSITIVES
		if(i==1) [[UIColor blackColor] set];
		if(i==2) [[UIColor orangeColor] set];
		if(i==3) [[UIColor magentaColor] set];
#endif
		//NSLog(@"ed:%.0f %.0f %.0f %.0f",r.origin.x,r.origin.y,r.size.width,r.size.height);

		CGContextStrokeRect(ctx, todrawEyes);
		
	}
}

- (void) reset {

	totFaces =0;
	totEyes =0;
	totMouths=0;
	totNoses=0;
	for(int i=0; i < 25; i++) {
		
		_faces[i].origin.x = 0;
		_faces[i].origin.y = 0;
		_faces[i].size.width = 0;
		_faces[i].size.height = 0;
		
		_eyes[i].origin.x = 0;
		_eyes[i].origin.y = 0;
		_eyes[i].size.width = 0;
		_eyes[i].size.height = 0;

		_noses[i].origin.x = 0;
		_noses[i].origin.y = 0;
		_noses[i].size.width = 0;
		_noses[i].size.height = 0;

		_mouth[i].origin.x = 0;
		_mouth[i].origin.y = 0;
		_mouth[i].size.width = 0;
		_mouth[i].size.height = 0;
		
	}
}

- (void)dealloc {
    [super dealloc];
}


@end
