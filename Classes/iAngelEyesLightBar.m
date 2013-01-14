//
//  iAngelEyesLightBar.m
//  iAngelEyes
//
//  Created by Ashish Lal on 17/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesLightBar.h"
#import "iAngelEyesController.h"

@implementation iAngelEyesLightBar

@synthesize delegate;
@synthesize backButton;
@synthesize L0Button;
@synthesize L1Button;
@synthesize L2Button;
@synthesize L3Button;
@synthesize L4Button;

- (id)initWithFrame:(CGRect)frame {
	
    if (self = [super initWithFrame:frame]) {

#if 0
		// create a bordered style button with custom title
		UIBarButtonItem *backButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"<<"
																	style:UIBarButtonItemStyleBordered	
																	target:self
																	action:@selector(goToMaterialPropertiesMore:)] autorelease];

		// create a bordered style button with custom title
		UIBarButtonItem *L0 = [[[UIBarButtonItem alloc] initWithTitle:@"L0"
																	style:UIBarButtonItemStyleBordered	
																	target:self
																	action:@selector(L0:)] autorelease];	
		
		// create a bordered style button with custom title
		UIBarButtonItem *L1 = [[[UIBarButtonItem alloc] initWithTitle:@"L1"
																style:UIBarButtonItemStyleBordered	
															   target:self
															   action:@selector(L1:)] autorelease];
		
		// create a bordered style button with custom title
		UIBarButtonItem *L2 = [[[UIBarButtonItem alloc] initWithTitle:@"L2"
																style:UIBarButtonItemStyleBordered	
															   target:self
															   action:@selector(L2:)] autorelease];
		
		UIBarButtonItem *L3 = [[[UIBarButtonItem alloc] initWithTitle:@"L3"
																style:UIBarButtonItemStyleBordered	
															   target:self
															   action:@selector(L3:)] autorelease];
		
		UIBarButtonItem *L4 = [[[UIBarButtonItem alloc] initWithTitle:@"L4"
																style:UIBarButtonItemStyleBordered	
															   target:self
															   action:@selector(L4:)] autorelease];
#else
		
		self.backButton = [UIButton buttonWithType:UIButtonTypeCustom];
		backButton.frame = CGRectMake(0, 0, 28, 30);
		backButton.adjustsImageWhenHighlighted = YES;
		[backButton setImage:[UIImage imageNamed:@"Backward.png"] forState:UIControlStateNormal];
        [backButton setImage:[UIImage imageNamed:@"Backward-depressed.png"] forState:UIControlStateHighlighted];
		[backButton addTarget:self action:@selector(goToMaterialPropertiesMore:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *backButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
		
		self.L0Button = [UIButton buttonWithType:UIButtonTypeCustom];
		L0Button.frame = CGRectMake(0, 0, 35, 30);
		L0Button.adjustsImageWhenHighlighted = YES;
		[L0Button setImage:[UIImage imageNamed:@"L0.png"] forState:UIControlStateNormal];
		[L0Button setImage:[UIImage imageNamed:@"L0-depressed.png"] forState:UIControlStateHighlighted];
		[L0Button addTarget:self action:@selector(L0:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *L0 = [[UIBarButtonItem alloc] initWithCustomView:L0Button];
		
		self.L1Button = [UIButton buttonWithType:UIButtonTypeCustom];
		L1Button.frame = CGRectMake(0, 0, 35, 30);
		L1Button.adjustsImageWhenHighlighted = YES;
		[L1Button setImage:[UIImage imageNamed:@"L1.png"] forState:UIControlStateNormal];
		[L1Button setImage:[UIImage imageNamed:@"L1-depressed.png"] forState:UIControlStateHighlighted];
		[L1Button addTarget:self action:@selector(L1:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *L1 = [[UIBarButtonItem alloc] initWithCustomView:L1Button];
		
		self.L2Button = [UIButton buttonWithType:UIButtonTypeCustom];
		L2Button.frame = CGRectMake(0, 0, 35, 30);
		L2Button.adjustsImageWhenHighlighted = YES;
		[L2Button setImage:[UIImage imageNamed:@"L2.png"] forState:UIControlStateNormal];
		[L2Button setImage:[UIImage imageNamed:@"L2-depressed.png"] forState:UIControlStateHighlighted];
		[L2Button addTarget:self action:@selector(L0:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *L2 = [[UIBarButtonItem alloc] initWithCustomView:L2Button];
		
		self.L3Button = [UIButton buttonWithType:UIButtonTypeCustom];
		L3Button.frame = CGRectMake(0, 0, 35, 30);
		L3Button.adjustsImageWhenHighlighted = YES;
		[L3Button setImage:[UIImage imageNamed:@"L3.png"] forState:UIControlStateNormal];
		[L3Button setImage:[UIImage imageNamed:@"L3-depressed.png"] forState:UIControlStateHighlighted];
		[L3Button addTarget:self action:@selector(L0:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *L3 = [[UIBarButtonItem alloc] initWithCustomView:L3Button];
		
		self.L4Button = [UIButton buttonWithType:UIButtonTypeCustom];
		L4Button.frame = CGRectMake(0, 0, 35, 30);
		L4Button.adjustsImageWhenHighlighted = YES;
		[L4Button setImage:[UIImage imageNamed:@"L4.png"] forState:UIControlStateNormal];
		[L4Button setImage:[UIImage imageNamed:@"L4-depressed.png"] forState:UIControlStateHighlighted];
		[L4Button addTarget:self action:@selector(L0:) forControlEvents:UIControlEventTouchUpInside];
		UIBarButtonItem *L4 = [[UIBarButtonItem alloc] initWithCustomView:L4Button];
#endif
		
	
        self.items = [NSArray arrayWithObjects:
					   backButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  L0,
                      L1,
					  L2,
					  L3,
					  L4,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      nil];
		
		[backButtonItem release];
		[L0 release];
		//[L0Button release];
		[L1 release];
		//[L1Button release];
		[L2 release];
		//[L2Button release];
		[L3 release];
		//[L3Button release];
		[L4 release];
		//[L4Button release];
        
    }
    
    return self;
}

- (void)dealloc {
	
	self.backButton = nil;
	self.L0Button = nil;
	self.L1Button = nil;
	self.L2Button = nil;
	self.L3Button = nil;
	self.L4Button = nil;
	
    [super dealloc];
}

- (void)goToMaterialPropertiesMore:(id)sender {
    if(delegate) {
    	[delegate goToMaterialPropertiesMore];
    }
}

- (void)L0:(id)sender {
    if(delegate) {
    	[delegate ToggleLight:0];
    }
}

- (void)L1:(id)sender {
    if(delegate) {
    	[delegate ToggleLight:1];
    }
}

- (void)L2:(id)sender {
    if(delegate) {
    	[delegate ToggleLight:2];
    }
}

- (void)L3:(id)sender {
    if(delegate) {
    	[delegate ToggleLight:3];
    }
}

- (void)L4:(id)sender {
    if(delegate) {
    	[delegate ToggleLight:4];
    }
}

- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
