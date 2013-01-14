//
//  PMToolbar.m
//  iAngelEyes
//

#import "iAngelEyesCameraBar.h"
#import "iAngelEyesController.h"


@implementation iAngelEyesCameraBar


@synthesize delegate;

@synthesize cameraButton;
@synthesize infoButton;
@synthesize statusLabel;

- (id)initWithFrame:(CGRect)frame {
    if(self = [super initWithFrame:frame]) {
        self.cameraButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 98, 40)];
        [cameraButton setImage:[UIImage imageNamed:@"camera-button.png"] forState:UIControlStateNormal];
        [cameraButton setImage:[UIImage imageNamed:@"camera-button-depressed.png"] forState:UIControlStateHighlighted];
        [cameraButton addTarget:self action:@selector(takePicture:) forControlEvents:UIControlEventTouchUpInside];
        
		self.infoButton;
		CGRect buttonSize = CGRectMake(0.0, 0.0, 30.0, 30.0);
		infoButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
		infoButton.frame = buttonSize;
		[infoButton addTarget:delegate action:@selector(showInfoPanel:) forControlEvents:UIControlEventTouchUpInside];
		
		//self.statusLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 140, 40)];
		self.statusLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 100, 40)];
        statusLabel.textAlignment = UITextAlignmentCenter;
        statusLabel.font = [UIFont boldSystemFontOfSize:15.0];
        statusLabel.textColor = [UIColor colorWithRed:0.126 green:0.126 blue:0.129 alpha:1.000];
        statusLabel.shadowColor = [UIColor colorWithRed:0.773 green:0.779 blue:0.793 alpha:1.000];
        statusLabel.backgroundColor = [UIColor clearColor];
        statusLabel.shadowOffset = CGSizeMake(0, 1);
        statusLabel.text = @"Idle";
        UIBarButtonItem *labelButtonItem = [[UIBarButtonItem alloc] initWithCustomView:statusLabel];
		
		
        UIBarButtonItem *cameraButtonItem = [[UIBarButtonItem alloc] initWithCustomView:cameraButton];
        UIBarButtonItem *infoBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:infoButton];
#if 0		
		UIBarButtonItem *libButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"PLib"
																		   style:UIBarButtonItemStyleBordered	
																		  target:self
																		  action:@selector(photoLibrary:)];
#endif		
        self.items = [NSArray arrayWithObjects:
                     // [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      labelButtonItem,
					  cameraButtonItem,
					  //libButtonItem,
					  //infoBarButtonItem
                      [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease],
                      infoBarButtonItem,
					  nil];
        
		[labelButtonItem release];
        //[cameraButton release];
        [cameraButtonItem release];  
		//[infoButton release];
        [infoBarButtonItem release]; 
		//[libButtonItem release]; 
    }
    
    return self;
}

- (void)dealloc {
	
	self.statusLabel = nil;
	self.cameraButton = nil;
	self.infoButton = nil;
	
    [super dealloc];
}

- (void)takePicture:(id)sender {
    if(delegate) {
    	[delegate takePicture];
    }
}

- (void) photoLibrary:(id) sender
{	
	if(delegate) {
    	[delegate photoLibrary];
    }
}	

- (void) showInfoPanel:(id) sender
{	
	if(delegate) {
    	[delegate showInfoPanel];
    }
}


- (void)drawRect:(CGRect)rect {
    [[UIImage imageNamed:@"toolbar-gradient.png"] drawInRect:rect];
}


@end
