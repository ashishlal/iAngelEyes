//
//  iAngelEyesPreviewBarCV.m
//  iAngelEyes
//
//  Created by Ashish Lal on 06/07/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesPreviewBarCV.h"
#import "iAngelEyesController.h"

@implementation iAngelEyesPreviewBarCV

@synthesize delegate;
@synthesize moveButton;
@synthesize rotateButton;
@synthesize resizeButton;
@synthesize forwardButton;

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
		
		
#if 0
		
		// create a bordered style button with custom title
		UIBarButtonItem *moveButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Move"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(movePicture:)] autorelease];
		
		
		// create a bordered style button with custom title
		UIBarButtonItem *rotateButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Rotate"
																			  style:UIBarButtonItemStyleBordered	
																			 target:self
																			 action:@selector(rotatePicture:)] autorelease];
		
		UIButtin *useButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 34, 40)];
        [useButton setImage:[UIImage imageNamed:@"use-button.png"] forState:UIControlStateNormal];
        [useButton setImage:[UIImage imageNamed:@"use-button-depressed.png"] forState:UIControlStateHighlighted];
        [useButton addTarget:self action:@selector(usePicture:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *useButtonItem = [[UIBarButtonItem alloc] initWithCustomView:useButton];  
		
		// create a bordered style button with custom title
		UIBarButtonItem *moreButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@">>"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(goToPreviewBarMore:)] autorelease];
#else
				
		self.moveButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 54, 30)];
		moveButton.adjustsImageWhenHighlighted = YES;
        [moveButton setImage:[UIImage imageNamed:@"Move.png"] forState:UIControlStateNormal];
        [moveButton setImage:[UIImage imageNamed:@"Move-depressed.png"] forState:UIControlStateHighlighted];
        [moveButton addTarget:self action:@selector(movePicture:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *moveButtonItem = [[UIBarButtonItem alloc] initWithCustomView:moveButton];
		
		self.rotateButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 54, 30)];
		rotateButton.adjustsImageWhenHighlighted = YES;
        [rotateButton setImage:[UIImage imageNamed:@"Rotate.png"] forState:UIControlStateNormal];
        [rotateButton setImage:[UIImage imageNamed:@"Rotate-depressed.png"] forState:UIControlStateHighlighted];
        [rotateButton addTarget:self action:@selector(rotatePicture:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *rotateButtonItem = [[UIBarButtonItem alloc] initWithCustomView:rotateButton];
		
		self.resizeButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 53, 30)];
		resizeButton.adjustsImageWhenHighlighted = YES;
        [resizeButton setImage:[UIImage imageNamed:@"Resize.png"] forState:UIControlStateNormal];
        [resizeButton setImage:[UIImage imageNamed:@"Resize-depressed.png"] forState:UIControlStateHighlighted];
        [resizeButton addTarget:self action:@selector(resizeObject:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *resizeButtonItem = [[UIBarButtonItem alloc] initWithCustomView:resizeButton];
		
		self.forwardButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 31, 30)];
		forwardButton.adjustsImageWhenHighlighted = YES;
        [forwardButton setImage:[UIImage imageNamed:@"Forward.png"] forState:UIControlStateNormal];
        [forwardButton setImage:[UIImage imageNamed:@"Forward-depressed.png"] forState:UIControlStateHighlighted];
        [forwardButton addTarget:self action:@selector(goToPreviewBarMore:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *forwardButtonItem = [[UIBarButtonItem alloc] initWithCustomView:forwardButton];
		
#endif
		
		
		
        self.items = [NSArray arrayWithObjects:
                      moveButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  //labelButtonItem,
                      rotateButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  resizeButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  forwardButtonItem,
                      nil];
		
		[rotateButtonItem release];
		[resizeButtonItem release];
		[forwardButtonItem release];
		[moveButtonItem release];
		
		//[retakeButton release];
		//[rotateButton release];
		//[resizeButton release];
		//[forwardButton release];
		
    }
    
    return self;
}


- (void)dealloc {
	
	self.moveButton = nil;
	self.rotateButton = nil;
	self.resizeButton = nil;
	self.forwardButton = nil;
	
    [super dealloc];
}


- (void)resizeObject:(id)sender {
    if(delegate) {
    	[delegate resizeObject];
    }
}


- (void)movePicture:(id)sender {
    if(delegate) {
    	[delegate movePicture];
    }
}

- (void)rotatePicture:(id)sender {
    if(delegate) {
    	[delegate rotatePicture];
    }
}

- (void)goToPreviewBarMore:(id)sender {
    if(delegate) {
    	[delegate goToPreviewBarMore];
    }
}


- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
