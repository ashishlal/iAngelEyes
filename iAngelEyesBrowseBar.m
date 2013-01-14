//
//  iAngelEyesBrowseBar.m
//  iAngelEyes
//
//  Created by Ashish Lal on 01/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import "iAngelEyesBrowseBar.h"
#import "iAngelEyesController.h"
#import "FaceAugmentingView.h"



@implementation iAngelEyesBrowseBar


@synthesize delegate;
@synthesize backButton;
@synthesize firstButton;
@synthesize prevButton;
@synthesize nextButton;
@synthesize lastButton;

- (id)initWithFrame:(CGRect)frame {
	
    if (self = [super initWithFrame:frame]) {
		
#if 1		
		self.backButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 30, 30)];
		backButton.adjustsImageWhenHighlighted = YES;
        [backButton setImage:[UIImage imageNamed:@"Backward.png"] forState:UIControlStateNormal];
        [backButton setImage:[UIImage imageNamed:@"Backward-depressed.png"] forState:UIControlStateHighlighted];
        [backButton addTarget:self action:@selector(goBackToPreviewBarMore:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *backButtonItem = [[UIBarButtonItem alloc] initWithCustomView:backButton];
		
		self.firstButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 40, 30)];
		firstButton.adjustsImageWhenHighlighted = YES;
        [firstButton setImage:[UIImage imageNamed:@"BrowseFirst.png"] forState:UIControlStateNormal];
        [firstButton setImage:[UIImage imageNamed:@"BrowseFirst-depressed.png"] forState:UIControlStateHighlighted];
        [firstButton addTarget:self action:@selector(firstObject:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *firstButtonItem = [[UIBarButtonItem alloc] initWithCustomView:firstButton];
		
		self.prevButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 40, 30)];
		prevButton.adjustsImageWhenHighlighted = YES;
        [prevButton setImage:[UIImage imageNamed:@"BrowsePrev.png"] forState:UIControlStateNormal];
        [prevButton setImage:[UIImage imageNamed:@"BrowsePrev-depressed.png"] forState:UIControlStateHighlighted];
        [prevButton addTarget:self action:@selector(prevObject:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *prevButtonItem = [[UIBarButtonItem alloc] initWithCustomView:prevButton];
		
		self.nextButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 40, 30)];
		nextButton.adjustsImageWhenHighlighted = YES;
        [nextButton setImage:[UIImage imageNamed:@"BrowseNext.png"] forState:UIControlStateNormal];
        [nextButton setImage:[UIImage imageNamed:@"BrowseNext-depressed.png"] forState:UIControlStateHighlighted];
        [nextButton addTarget:self action:@selector(nextObject:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *nextButtonItem = [[UIBarButtonItem alloc] initWithCustomView:nextButton];
		
		self.lastButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 40, 30)];
		lastButton.adjustsImageWhenHighlighted = YES;
        [lastButton setImage:[UIImage imageNamed:@"BrowseLast.png"] forState:UIControlStateNormal];
        [lastButton setImage:[UIImage imageNamed:@"BrowseLast-depressed.png"] forState:UIControlStateHighlighted];
        [lastButton addTarget:self action:@selector(lastObject:) forControlEvents:UIControlEventTouchUpInside];
        UIBarButtonItem *lastButtonItem = [[UIBarButtonItem alloc] initWithCustomView:lastButton];
#else		
		// create a bordered style button with custom title
		UIBarButtonItem *backButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Back"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(goBackToPreviewBarMore:)] autorelease];
		
	
		// create a bordered style button with custom title
		UIBarButtonItem *firstButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"|<<"
																			  style:UIBarButtonItemStyleBordered	
																			 target:self
																			 action:@selector(firstObject:)] autorelease];
		
		
		// create a bordered style button with custom title
		UIBarButtonItem *prevButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"<<"
																			  style:UIBarButtonItemStyleBordered	
																			 target:self
																			 action:@selector(prevObject:)] autorelease];
		
		
		// create a bordered style button with custom title
		UIBarButtonItem *nextButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@">>"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(nextObject:)] autorelease];		
		
        
		// create a bordered style button with custom title
		UIBarButtonItem *lastButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@">>|"
																			style:UIBarButtonItemStyleBordered	
																		   target:self
																		   action:@selector(lastObject:)] autorelease];
#endif 			
        self.items = [NSArray arrayWithObjects:
					   backButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
					  firstButtonItem,
					  prevButtonItem,
					  nextButtonItem,
                      lastButtonItem,
					  [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      nil];
       
		[backButtonItem release];
		//[backButton release];
		[firstButtonItem release];
		//[firstButton release];
		[prevButtonItem release];
		//[prevButton release];
		[nextButtonItem release];
		[lastButtonItem release];
		//[lastButton release];
    }
    
    return self;
}


- (void)dealloc {
    
	self.backButton = nil;
	self.firstButton = nil;
	self.prevButton = nil;
	self.nextButton = nil;
	self.lastButton = nil;
	
    [super dealloc];
}


- (void)goBackToPreviewBarMore:(id)sender {
    if(delegate) {
    	[delegate goToPreviewBarMore];
    }
}


- (void)firstObject:(id)sender {
    if(delegate) {
    	[delegate firstObject];
    }
}

- (void)prevObject:(id)sender {
    if(delegate) {
    	[delegate prevObject];
    }
}

- (void)nextObject:(id)sender {
    if(delegate) {
    	[delegate nextObject];
    }
}

- (void)lastObject:(id)sender {
    if(delegate) {
    	[delegate lastObject];
    }
}


- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
