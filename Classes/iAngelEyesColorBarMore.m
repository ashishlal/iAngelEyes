//
//  iAngelEyesColorBarMore.m
//  iAngelEyes
//
//  Created by Ashish Lal on 04/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesColorBarMore.h"
#import "iAngelEyesController.h"

@implementation iAngelEyesColorBarMore

@synthesize delegate;
@synthesize backButton;
@synthesize specularButton;
@synthesize shininessButton;
//@synthesize lightButton;

- (id)initWithFrame:(CGRect)frame {
	
    if (self = [super initWithFrame:frame]) {

#if 0
		// create a bordered style button with custom title
		UIBarButtonItem *backButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Back"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(goToMaterialProperties:)] autorelease];
	
		
		// create a bordered style button with custom title
		UIBarButtonItem *specularButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Specular"
																				style:UIBarButtonItemStyleBordered	
																			   target:self
																			   action:@selector(ObjectSpecular:)] autorelease];		
		
        
		// create a bordered style button with custom title
		UIBarButtonItem *shininessButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Shininess"
																				 style:UIBarButtonItemStyleBordered	
																				target:self
																				action:@selector(ObjectShininess:)] autorelease];
#if 0	
		// create a bordered style button with custom title
		UIBarButtonItem *resetButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Reset"
																			 style:UIBarButtonItemStyleBordered	
																			target:self
																			action:@selector(ObjectColorReset:)] autorelease];
#else
		UIBarButtonItem *lightButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Light"
																			 style:UIBarButtonItemStyleBordered	
																			target:self
																			action:@selector(goToLightBar:)] autorelease];
#endif
#else
		self.backButton = [UIButton buttonWithType:UIButtonTypeCustom];
		backButton.frame = CGRectMake(0, 0, 28, 30);
		backButton.adjustsImageWhenHighlighted = YES;
		[backButton setImage:[UIImage imageNamed:@"Backward.png"] forState:UIControlStateNormal];
        [backButton setImage:[UIImage imageNamed:@"Backward-depressed.png"] forState:UIControlStateHighlighted];
		[backButton addTarget:self action:@selector(goToMaterialProperties:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *backButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
		
		self.specularButton = [UIButton buttonWithType:UIButtonTypeCustom];
		specularButton.frame = CGRectMake(0, 0, 60, 30);
		specularButton.adjustsImageWhenHighlighted = YES;
		[specularButton setImage:[UIImage imageNamed:@"Specular.png"] forState:UIControlStateNormal];
        [specularButton setImage:[UIImage imageNamed:@"Specular-depressed.png"] forState:UIControlStateHighlighted];
		[specularButton addTarget:self action:@selector(ObjectSpecular:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *specularButtonItem = [[UIBarButtonItem alloc] initWithCustomView:specularButton];
				
		self.shininessButton = [UIButton buttonWithType:UIButtonTypeCustom];
		shininessButton.frame = CGRectMake(0, 0, 54, 30);
		shininessButton.adjustsImageWhenHighlighted = NO;
		[shininessButton setImage:[UIImage imageNamed:@"Shininess.png"] forState:UIControlStateNormal];
        [shininessButton setImage:[UIImage imageNamed:@"Shininess-depressed.png"] forState:UIControlStateHighlighted];
		[shininessButton addTarget:self action:@selector(ObjectShininess:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *shininessButtonItem = [[UIBarButtonItem alloc] initWithCustomView:shininessButton];
#if 0		
		self.lightButton = [UIButton buttonWithType:UIButtonTypeCustom];
		lightButton.frame = CGRectMake(0, 0, 54, 30);
		lightButton.adjustsImageWhenHighlighted = NO;
		[lightButton setImage:[UIImage imageNamed:@"Light.png"] forState:UIControlStateNormal];
        [lightButton setImage:[UIImage imageNamed:@"Light-depressed.png"] forState:UIControlStateHighlighted];
		[lightButton addTarget:self action:@selector(goToLightBar:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *lightButtonItem = [[UIBarButtonItem alloc] initWithCustomView:lightButton];
#endif
#endif
		

        self.items = [NSArray arrayWithObjects:
					  backButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  specularButtonItem,
                      shininessButtonItem,
					  //resetButtonItem,
					  //lightButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      nil];
        
		[backButtonItem release]; 
		[specularButtonItem release]; 
		[shininessButtonItem release]; 
		//[lightButtonItem release];
		
       // [backButton release];
		//[specularButton release];
		//[shininessButton release];
		//[lightButton release];
	}
    
    return self;
}


- (void)dealloc {
	
	self.backButton = nil;
	self.specularButton = nil;
	self.shininessButton = nil;
	//self.lightButton = nil;
	
    [super dealloc];
}


- (void)goToMaterialProperties:(id)sender {
    if(delegate) {
    	[delegate goToMaterialProperties];
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

- (void)ObjectColorReset:(id)sender {
    if(delegate) {
    	[delegate ObjectColorReset];
    }
}

- (void)goToLightBar:(id)sender {
    if(delegate) {
    	[delegate goToLightBar];
    }
}


- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
