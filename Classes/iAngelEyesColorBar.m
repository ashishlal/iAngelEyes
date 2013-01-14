//
//  iAngelEyesColorBar.m
//  iAngelEyes
//
//  Created by Ashish Lal on 02/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesColorBar.h"
#import "iAngelEyesController.h"

@implementation iAngelEyesColorBar

@synthesize delegate;
@synthesize backButton;
@synthesize textureButton;
@synthesize ambienceButton;
@synthesize diffusionButton;
@synthesize forwardButton;

- (id)initWithFrame:(CGRect)frame {
	
    if (self = [super initWithFrame:frame]) {
		
#if 1		
		self.backButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 28, 30)];
		backButton.adjustsImageWhenHighlighted = YES;
        [backButton setImage:[UIImage imageNamed:@"Backward.png"] forState:UIControlStateNormal];
        [backButton setImage:[UIImage imageNamed:@"Backward-depressed.png"] forState:UIControlStateHighlighted];
        [backButton addTarget:self action:@selector(goBackToPreviewBarMore:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *backButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
#if 0		
		self.textureButton = [UIButton buttonWithType:UIButtonTypeCustom];
		textureButton.frame = CGRectMake(0, 0, 54, 30);
		textureButton.adjustsImageWhenHighlighted = YES;
		[textureButton setImage:[UIImage imageNamed:@"Texture.png"] forState:UIControlStateNormal];
        [textureButton setImage:[UIImage imageNamed:@"Texture-depressed.png"] forState:UIControlStateHighlighted];
		[textureButton addTarget:self action:@selector(ObjectTexture:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *textureButtonItem = [[UIBarButtonItem alloc] initWithCustomView:textureButton];
#endif		
		self.ambienceButton = [UIButton buttonWithType:UIButtonTypeCustom];
		ambienceButton.frame = CGRectMake(0, 0, 54, 30);
		ambienceButton.adjustsImageWhenHighlighted = YES;
		[ambienceButton setImage:[UIImage imageNamed:@"Ambience.png"] forState:UIControlStateNormal];
        [ambienceButton setImage:[UIImage imageNamed:@"Ambience-depressed.png"] forState:UIControlStateHighlighted];
		[ambienceButton addTarget:self action:@selector(ObjectAmbience:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *ambienceButtonItem = [[UIBarButtonItem alloc] initWithCustomView:ambienceButton];
		
		self.diffusionButton = [UIButton buttonWithType:UIButtonTypeCustom];
		diffusionButton.frame = CGRectMake(0, 0, 54, 30);
		diffusionButton.adjustsImageWhenHighlighted = YES;
		[diffusionButton setImage:[UIImage imageNamed:@"Diffusion.png"] forState:UIControlStateNormal];
        [diffusionButton setImage:[UIImage imageNamed:@"Diffusion-depressed.png"] forState:UIControlStateHighlighted];
		[diffusionButton addTarget:self action:@selector(ObjectDiffusion:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *diffusionButtonItem = [[UIBarButtonItem alloc] initWithCustomView:diffusionButton];

		self.forwardButton = [UIButton buttonWithType:UIButtonTypeCustom];
		forwardButton.frame = CGRectMake(0, 0, 28, 30);
		forwardButton.adjustsImageWhenHighlighted = YES;
		[forwardButton setImage:[UIImage imageNamed:@"Forward.png"] forState:UIControlStateNormal];
        [forwardButton setImage:[UIImage imageNamed:@"Forward-depressed.png"] forState:UIControlStateHighlighted];
		[forwardButton addTarget:self action:@selector(goToMaterialPropertiesMore:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *forwardButtonItem = [[UIBarButtonItem alloc] initWithCustomView:forwardButton];
		
#else
		
		// create a bordered style button with custom title
		UIBarButtonItem *backButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Back"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(goBackToPreviewBarMore:)] autorelease];
		
		// create a bordered style button with custom title
		UIBarButtonItem *textureButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Tex"
																			   style:UIBarButtonItemStyleBordered	
																			  target:self
																			  action:@selector(ObjectTexture:)] autorelease];
		
		// create a bordered style button with custom title
		UIBarButtonItem *ambienceButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Amb"
																			 style:UIBarButtonItemStyleBordered	
																			target:self
																			action:@selector(ObjectAmbience:)] autorelease]; 
		
		// create a bordered style button with custom title
		UIBarButtonItem *diffusionButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Diff"
																				 style:UIBarButtonItemStyleBordered	
																				target:self
																				action:@selector(ObjectDiffusion:)] autorelease];
		
		UIBarButtonItem *morePropetriesButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"More"
																					  style:UIBarButtonItemStyleBordered	
																					 target:self
																					 action:@selector(goToMaterialPropertiesMore:)] autorelease];
		
#endif		

			
 
        self.items = [NSArray arrayWithObjects:
					  backButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  //textureButtonItem,
					  ambienceButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  diffusionButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      forwardButtonItem,
					  nil];
		
		
		[backButtonItem release];
		[ambienceButtonItem release]; 
		[diffusionButtonItem release]; 
		//[textureButtonItem release]; 
		[forwardButtonItem release]; 
		
		//[backButton release];
        //[ambienceButton release];
		//[textureButton release];
		//[diffusionButton release];
		//[forwardButton release];
    }

	 
    return self;
}


- (void)dealloc {
	
	self.backButton = nil;
	self.textureButton = nil;
	self.ambienceButton = nil;
	self.diffusionButton = nil;
	self.forwardButton = nil;
	
    [super dealloc];
}


- (void)goBackToPreviewBarMore:(id)sender {
    if(delegate) {
    	[delegate goToPreviewBarMore];
    }
}

- (void)goToMaterialPropertiesMore:(id)sender {
    if(delegate) {
    	[delegate goToMaterialPropertiesMore];
    }
}

- (void)ObjectTexture:(id)sender {
    if(delegate) {
    	[delegate ObjectTexture];
    }
}

- (void)ObjectAmbience:(id)sender {
    if(delegate) {
    	[delegate ObjectAmbience];
    }
}

- (void)ObjectDiffusion:(id)sender {
    if(delegate) {
    	[delegate ObjectDiffusion];
    }
}

- (void)ObjectSpecular:(id)sender {
    if(delegate) {
    	[delegate ObjectSpecular];
    }
}

- (void)ObjectShininess:(id)sender {
    if(delegate) {
    	[delegate ObjectShininess];
    }
}


- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
