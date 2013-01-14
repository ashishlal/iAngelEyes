//
//  iAngelEyesImageView.m
//  iAngelEyes
//

#import "iAngelEyesImageView.h"


@implementation iAngelEyesImageView


@synthesize face;
@synthesize image;


- (void)dealloc {
    self.image = nil;
    
    [super dealloc];
}


- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];

    [image drawInRect:rect];
    
    /*
    if(face.size.width > 0 && face.size.height > 0) {
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGFloat radius = 10.0;
        CGFloat minx = CGRectGetMinX(face);
        CGFloat midx = CGRectGetMidX(face);
        CGFloat maxx = CGRectGetMaxX(face);
        CGFloat miny = CGRectGetMinY(face);
        CGFloat midy = CGRectGetMidY(face);
        CGFloat maxy = CGRectGetMaxY(face);
        CGContextMoveToPoint(context, minx, midy);
        CGContextAddArcToPoint(context, minx, miny, midx, miny, radius);
        CGContextAddArcToPoint(context, maxx, miny, maxx, midy, radius);
        CGContextAddArcToPoint(context, maxx, maxy, midx, maxy, radius);
        CGContextAddArcToPoint(context, minx, maxy, minx, midy, radius);
        CGContextClosePath(context);
        CGContextSetLineWidth(context, 3.0);
        CGContextSetStrokeColorWithColor(context, [UIColor greenColor].CGColor);
        CGContextDrawPath(context, kCGPathStroke);
    }
    */
}


@end
