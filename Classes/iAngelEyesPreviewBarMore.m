//
//  iAngelEyesPreviewBarMore.m
//  iAngelEyes
//
//  Created by Ashish Lal on 27/05/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesPreviewBarMore.h"

#import "iAngelEyesController.h"

@implementation iAngelEyesPreviewBarMore

@synthesize delegate;
@synthesize backButton;
@synthesize browseButton;
@synthesize colorButton;
@synthesize saveButton;

- (id)initWithFrame:(CGRect)frame {
	
    if (self = [super initWithFrame:frame]) {
		
#if 0	
		// create a bordered style button with custom title
		UIBarButtonItem *backButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"<<"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(goBackToPreviewBar:)] autorelease];
		
		// create a bordered style button with custom title
		UIBarButtonItem *resizeButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Resize"
																	style:UIBarButtonItemStyleBordered	
																	target:self
																	action:@selector(resizeObject:)] autorelease];
		
		
		// create a bordered style button with custom title
		UIBarButtonItem *browseButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Browse"
																	style:UIBarButtonItemStyleBordered	
																	target:self
																	action:@selector(browseObjects:)] autorelease];
		
		// create a bordered style button with custom title
		UIBarButtonItem *colorButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Color"
																		style:UIBarButtonItemStyleBordered	
																		target:self
																		action:@selector(goToMaterialProperties:)] autorelease];
		
		
		
		// create a bordered style button with custom title
		UIBarButtonItem *moreButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@">>"
																	style:UIBarButtonItemStyleBordered	
																	target:self
																	action:@selector(goToMaterialProperties:)] autorelease];		
		
#else
		self.backButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 28, 30)];
		backButton.adjustsImageWhenHighlighted = YES;
        [backButton setImage:[UIImage imageNamed:@"Backward.png"] forState:UIControlStateNormal];
        [backButton setImage:[UIImage imageNamed:@"Backward-depressed.png"] forState:UIControlStateHighlighted];
        [backButton addTarget:self action:@selector(goBackToPreviewBar:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *backButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
		
		self.browseButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 54, 30)];
		browseButton.adjustsImageWhenHighlighted = YES;
        [browseButton setImage:[UIImage imageNamed:@"Browse.png"] forState:UIControlStateNormal];
        [browseButton setImage:[UIImage imageNamed:@"Browse-depressed.png"] forState:UIControlStateHighlighted];
        [browseButton addTarget:self action:@selector(browseObjects:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *browseButtonItem = [[UIBarButtonItem alloc] initWithCustomView:browseButton];

		self.colorButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 54, 30)];
		colorButton.adjustsImageWhenHighlighted = YES;
        [colorButton setImage:[UIImage imageNamed:@"Color.png"] forState:UIControlStateNormal];
        [colorButton setImage:[UIImage imageNamed:@"Color-depressed.png"] forState:UIControlStateHighlighted];
        [colorButton addTarget:self action:@selector(goToMaterialProperties:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *colorButtonItem = [[UIBarButtonItem alloc] initWithCustomView:colorButton];
		
		self.saveButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 54, 30)];
		saveButton.adjustsImageWhenHighlighted = YES;
        [saveButton setImage:[UIImage imageNamed:@"Save.png"] forState:UIControlStateNormal];
        [saveButton setImage:[UIImage imageNamed:@"Save-depressed.png"] forState:UIControlStateHighlighted];
        [saveButton addTarget:self action:@selector(usePicture:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *saveButtonItem = [[UIBarButtonItem alloc] initWithCustomView:saveButton];
		
#endif
        self.items = [NSArray arrayWithObjects:
					  backButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  
					  browseButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  
					  colorButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  
					  saveButtonItem,
                      //moreButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      nil];
        
		
		[backButtonItem release];
		[browseButtonItem release];
		[colorButtonItem release];
		[saveButtonItem release];
		
		//[backButton release];
		//[browseButton release];
		//[colorButton release];
		//[saveButton release];
		
    }
    
	
	
    return self;
}


- (void)dealloc {
	
	self.backButton = nil;
	self.browseButton = nil;
	self.colorButton = nil;
	self.saveButton = nil;
	
    [super dealloc];
}



- (void)browseObjects:(id)sender {
    if(delegate) {
    	[delegate browseObjects];
    }
}

- (void)goToMaterialProperties:(id)sender {
    if(delegate) {
    	[delegate goToMaterialProperties];
    }
}

- (void)goBackToPreviewBar:(id)sender {
    if(delegate) {
    	[delegate goBackToPreviewBar];
    }
}

- (void)usePicture:(id)sender {
    if(delegate) {
    	[delegate usePicture];
    }
}

- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
