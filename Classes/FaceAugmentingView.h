//
//  FaceAugmentingView.h
//  FaceDetector1
//

#import <UIKit/UIKit.h>

enum FACE_AUGMENT {
	HAT = 0,TIE = 1,BANDANA = 2, RECTANGLE=3
};

@interface FaceAugmentingView : UIView {
	int whatToPut;
	CGRect _faces[25];
	CGRect _noses[25];
	CGRect _eyes[25];
	CGRect _mouth[25];
		
	BOOL showRect;
	int totFaces, totNoses, totEyes, totMouths;
}

@property (readonly) CGRect *faces;
@property (readonly) CGRect *noses;
@property (readonly) CGRect *eyes;
@property (readonly) CGRect *mouth;
@property (nonatomic,assign) int whatToPut;
@property (nonatomic,assign) int totFaces, totNoses, totEyes, totMouths;

-(void)toggleShowRect:(int)val;
-(void) drawFace:(CGContextRef)ctx;
-(void) drawNose:(CGContextRef)ctx;
-(void) drawMouth:(CGContextRef)ctx;
-(void) drawEyes:(CGContextRef)ctx;
-(void) reset;
@end
