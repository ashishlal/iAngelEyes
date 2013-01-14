#import "InfoViewController.h"



#define CANCEL_IMPORT_TITLE @"Cancel import?"


@interface InfoViewController (Private)

- (void) adjustContentToOrientation:(UIInterfaceOrientation)newOrientation animated:(BOOL)animated;
- (void) switchToTab:(NSUInteger)tab;
- (void) lockGUI:(BOOL)lock;
- (void) newEponymsAreAvailable:(BOOL)available;
- (void) resetStatusElementsWithButtonTitle:(NSString *)buttonTitle;

@end




@implementation InfoViewController

@synthesize delegate, firstTimeLaunch, lastCheck, lastUpdate, tabSegments, infoPlistDict, projectWebsiteURL;
@synthesize infoView;
@synthesize updatesView;
@synthesize optionsView;
@synthesize settingsView;
@synthesize helpView;
@synthesize backgroundImage;


// Info
@synthesize versionLabel;
@synthesize usingLabel;
@synthesize authorTextView;
@synthesize propsTextView;

//IBOutlet UIButton *projectWebsiteButton;
//IBOutlet UIButton *eponymsDotNetButton;

// Updates
@synthesize lastCheckLabel;
@synthesize lastUpdateLabel;

@synthesize progressText;
@synthesize progressView;

@synthesize updateButton;
@synthesize autocheckSwitch;

// Options
@synthesize alphaCorrectionSwitch;
@synthesize threeGSFactorSwitch;
@synthesize loopImageCaptureSwitch;
@synthesize showOutlineSwitch;
@synthesize useCameraAR;

//settings
@synthesize useQuickestSearchSwitch;
@synthesize preserveOrientationSwitch;
@synthesize disableFDModuleSwitch;
@synthesize maxAttempts;

//Help
@synthesize helpScroll;
@synthesize helpFile;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

	if(self) {
		askingToAbortImport = NO;
		
		// compose the navigation bar
	//	NSArray *possibleTabs = [NSArray arrayWithObjects:@"About", @"Update", @"Options", @"Help", nil];
		NSArray *possibleTabs = [NSArray arrayWithObjects:@"About", @"Options", @"Advanced", @"Help", nil];
		self.tabSegments = [[UISegmentedControl alloc] initWithItems:possibleTabs];
		tabSegments.selectedSegmentIndex = 0;
		tabSegments.segmentedControlStyle = UISegmentedControlStyleBar;
		tabSegments.frame = CGRectMake(0.0, 0.0, 240.0, 30.0);
		tabSegments.autoresizingMask = UIViewAutoresizingFlexibleWidth;
		[tabSegments addTarget:self action:@selector(tabChanged:) forControlEvents:UIControlEventValueChanged];
		
		self.navigationItem.titleView	= tabSegments;
		self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Done" style:UIBarButtonItemStyleDone target:self action:@selector(dismissMe:)] autorelease];
		
		// add observer for the respective notifications (depending on the os version)
		if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 3.2) {
			[[NSNotificationCenter defaultCenter] addObserver:self 
												selector:@selector(keyboardDidShow:) 
												name:UIKeyboardDidShowNotification 
												object:nil];		
		} else {
			[[NSNotificationCenter defaultCenter] addObserver:self 
												selector:@selector(keyboardWillShow:) 
												name:UIKeyboardWillShowNotification 
												object:nil];
		}
		
		// NSBundle Info.plist
		self.infoPlistDict = [[NSBundle mainBundle] infoDictionary];		// !! could use the supplied NSBundle or the mainBundle on nil
		//self.projectWebsiteURL = [NSURL URLWithString:[infoPlistDict objectForKey:@"projectWebsite"]];
	}

	return self;
}

- (void) dealloc
{
	self.infoPlistDict = nil;
	self.projectWebsiteURL = nil;
	self.tabSegments = nil;
	
	// IBOutlets
	[infoView release];
	[updatesView release];
	[optionsView release];
	[backgroundImage release];
	
	[versionLabel release];
	//[usingEponymsLabel release];
	[authorTextView release];
	[propsTextView release];
	
	//[projectWebsiteButton release];
	//[eponymsDotNetButton release];
	
	[lastCheckLabel release];
	[lastUpdateLabel release];
	
	[progressText release];
	[progressView release];
	
	[updateButton release];
	[autocheckSwitch release];
	
	[alphaCorrectionSwitch release];
	[threeGSFactorSwitch release];
	[loopImageCaptureSwitch release];
	[useCameraAR release];
	
	[useQuickestSearchSwitch release];
	[preserveOrientationSwitch release];
	[disableFDModuleSwitch release];
	[maxAttempts release];
	
	[super dealloc];
}
#pragma mark -



#pragma mark View Controller Delegate
- (void) viewDidLoad
{
	self.view = infoView;
	
	tabSegments.tintColor = [delegate naviBarTintColor];
	[self switchToTab:0];
	lastInterfaceOrientation = UIInterfaceOrientationPortrait;
	
	// hide progress stuff
	//[self setStatusMessage:nil];
	//[self resetStatusElementsWithButtonTitle:nil];
	
	//projectWebsiteButton.autoresizingMask = UIViewAutoresizingNone;
	//eponymsDotNetButton.autoresizingMask = UIViewAutoresizingNone;
	propsTextView.autoresizingMask = UIViewAutoresizingNone;
	
	helpScroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 10, 320, 460)];
	helpFile =     [[UIImageView alloc] initWithFrame:CGRectMake(0, 10, 320, 1860)];
	[helpFile setImage:[UIImage imageNamed:@"Help-AngelEyes.png"]];  
	helpScroll = [[UIScrollView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	helpScroll.pagingEnabled = YES;
	helpScroll.delegate=self;
	helpScroll.bounces = YES;
	helpScroll.backgroundColor = [UIColor clearColor];
	helpScroll.scrollEnabled=YES;
	helpScroll.contentMode = (UIViewContentModeScaleAspectFit);
	//scroll.delegate=self;
	helpScroll.contentSize = CGSizeMake(helpFile.frame.size.width, helpFile.frame.size.height);
	helpScroll.clipsToBounds =YES;
	[helpScroll addSubview:helpFile];
    [helpView addSubview:helpScroll];
	
	// version
	//NSString *version = [NSString stringWithFormat:@"Version %@  (%@)", [infoPlistDict objectForKey:@"CFBundleVersion"], [infoPlistDict objectForKey:@"SubversionRevision"]];
	NSString *version = @"Version 1.0.0";
	[versionLabel performSelectorOnMainThread : @ selector(setText : ) withObject:version waitUntilDone:YES];
#if 1	
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	[dateFormatter setDateStyle:NSDateFormatterShortStyle];
	[dateFormatter setTimeStyle:NSDateFormatterShortStyle];
	[dateFormatter setTimeStyle:NSDateFormatterNoStyle];
	NSDate *today = [NSDate date];
	
	NSString *usingString = ([today timeIntervalSince1970] > 10.0) ? [dateFormatter stringFromDate:today] : @"Unknown";
	NSString *myDateStr = @"Date:";
	//NSString *myDate = [NSString stringWithFormat:@"%@%@", myDateStr, usingString];
	NSString *myDate = [[myDateStr stringByAppendingString:@" "] stringByAppendingString:usingString];
	//NSString *myDate = [myDateStr stringByAppendingString:usingString];
	//[usingLabel setText:[NSString stringWithFormat:@"Date: %@", usingString]];
	[usingLabel performSelectorOnMainThread : @ selector(setText : ) withObject:myDate waitUntilDone:YES];
	[dateFormatter release];
#endif
}

- (void) viewWillAppear:(BOOL)animated
{
	BOOL mustSeeProgress = YES;
	//firstTimeLaunch || [delegate newEponymsAvailable];
	firstTimeLaunch = FALSE;
	if(mustSeeProgress) {
		[self switchToTab:1];
	}
}

- (void) viewDidAppear:(BOOL)animated
{
	if(firstTimeLaunch) {
		NSString *title = @"First Launch";
		NSString *message = @"Welcome to iAngelEyes! Please wait For Initialization..";
		
		[self alertViewWithTitle:title message:message cancelTitle:@"OK"];		// maybe allow postponing first import?
	}
	
	// Adjust options
	//autocheckSwitch.on = [delegate shouldAutoCheck];
	
	[self adjustContentToOrientation:[self interfaceOrientation] animated:NO];
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
#if 0
	if(((eponyms_touchAppDelegate *)[[UIApplication sharedApplication] delegate]).allowAutoRotate) {
		[self adjustContentToOrientation:interfaceOrientation animated:YES];
		return YES;
	}
#endif	
	return ((interfaceOrientation == UIInterfaceOrientationPortrait) || (interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown));
	
}

- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
}

- (void) adjustContentToOrientation:(UIInterfaceOrientation)newOrientation animated:(BOOL)animated
{
	if(newOrientation != lastInterfaceOrientation) {
		//CGPoint websiteCenter;
		//CGPoint eponymsNetCenter;
		CGRect propsFrame;
		
		// get dimensions
		CGSize screenSize;// = infoView.frame.size;		// can't use this here since upon calling infoView still has the old dimensions
		CGRect foo = authorTextView.frame;
		CGPoint authorsOrigin = foo.origin;
		CGFloat authorsHeight = foo.size.height;
		
		// to Portrait
		if((UIInterfaceOrientationPortrait == newOrientation) || (UIInterfaceOrientationPortraitUpsideDown == newOrientation)) {
			screenSize = CGSizeMake(320, 416);
#if 0
			websiteCenter = CGPointMake(roundf((screenSize.width - 48.0) / 4) + 20.0,			// - 48 = - 2*20 (margin) + -8 (space between buttons)
										authorsOrigin.y + authorsHeight 
										//+ (projectWebsiteButton.bounds.size.height / 2) 
										+ 10);
			eponymsNetCenter = CGPointMake(roundf((screenSize.width - 48.0) / 4 * 3) + 28.0,	// + 28 = + 20 (margin) + 8 (space between buttons)
										   authorsOrigin.y + authorsHeight + 
										//   (projectWebsiteButton.bounds.size.height / 2) 
										   + 10);
#endif
			CGFloat propsFrameY = authorsOrigin.y + authorsHeight +
				//projectWebsiteButton.bounds.size.height 
				+ 20;
			propsFrame = CGRectMake(20,
									propsFrameY,
									screenSize.width - 40,
									screenSize.height - propsFrameY - 20);
		}
		
		// Landscape
		else {
			screenSize = CGSizeMake(480, 268);
			//websiteCenter = CGPointMake(screenSize.width - roundf((projectWebsiteButton.bounds.size.width / 2) + 20), 38.5);
			//eponymsNetCenter = CGPointMake(screenSize.width - roundf((eponymsDotNetButton.bounds.size.width / 2) + 20), 86.5);
			CGFloat propsFrameY = authorsOrigin.y + authorsHeight;
			propsFrame = CGRectMake(20,
									propsFrameY,
									screenSize.width - 40,
									screenSize.height - propsFrameY - 20);
		}
		
		// Start animation
		if(animated) {
			[UIView beginAnimations:nil context:nil];
			
			//projectWebsiteButton.center = websiteCenter;
			//eponymsDotNetButton.center = eponymsNetCenter;
			propsTextView.frame = propsFrame;
			
			[UIView commitAnimations];
		}
		else {
			//projectWebsiteButton.center = websiteCenter;
			//eponymsDotNetButton.center = eponymsNetCenter;
			propsTextView.frame = propsFrame;
		}
	}
	
	lastInterfaceOrientation = newOrientation;
}

- (void) didReceiveMemoryWarning
{
	[self dismissMe:nil];
	[super didReceiveMemoryWarning];		// Releases the view if it doesn't have a superview !!
}

- (void) dismissMe:(id)sender
{
	// warning when closing during import
	//if([delegate AmIUpdating]) {
	////	askingToAbortImport = YES;
	//	NSString *warning = @"Are you sure you want to abort the eponym import? This will discard any imported eponyms.";
	//	[self alertViewWithTitle:CANCEL_IMPORT_TITLE message:warning cancelTitle:@"Continue" otherTitle:@"Abort Import"];
	//}
	
	// not importing
	//else {
		[self.parentViewController dismissModalViewControllerAnimated:YES];
	//}
	[delegate dismissInfoScreen];
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView{
	return optionsView;
}

#pragma mark -



#pragma mark GUI
- (void) tabChanged:(id)sender
{
	UISegmentedControl *segment = sender;
	[self switchToTab:segment.selectedSegmentIndex];
}

- (void) switchToTab:(NSUInteger)tab
{
	tabSegments.selectedSegmentIndex = tab;
	if([backgroundImage superview]) {
		[backgroundImage removeFromSuperview];
	}
	
	// Show the About page
	if(0 == tab) {
		self.view = infoView;
	}
#if 0
	// Show the update tab
	else if(1 == tab) {
		self.view = updatesView;
		
		// adjust the elements
	//	if([delegate didCheckForNewEponyms]) {
	//		[self newEponymsAreAvailable:[delegate newEponymsAvailable]];
	//	}
	}
	
	// Show the options
	else if(2 == tab) {
		self.view = optionsView;
		
		// adjust the elements
		//allowRotateSwitch.on = ((eponyms_touchAppDelegate *)[[UIApplication sharedApplication] delegate]).allowAutoRotate;
	}
	else if(3 == tab) {
		self.view = helpView;
		
		// adjust the elements
		//allowRotateSwitch.on = ((eponyms_touchAppDelegate *)[[UIApplication sharedApplication] delegate]).allowAutoRotate;
	}
#else
	else if(1 == tab) {
		self.view = optionsView;
		
	}
	else if(2 == tab) {
		self.view = settingsView;
		
	}
	// Show the options
	else if(3 == tab) {
		self.view = helpView;
		
	}
	
#endif
	
	[self.view insertSubview:backgroundImage atIndex:0];
	[delegate setNavController];
}

- (void) newEponymsAreAvailable:(BOOL)available
{
	NSString *statusMessage = nil;
	if(available) {
		statusMessage = @"New eponyms are available!";
		[self setUpdateButtonTitle:@"Download New Eponyms"];
		[self setUpdateButtonTitleColor:[UIColor redColor]];
		[self setProgress:-1.0];
	}
	else {
		statusMessage = @"You are up to date";
		[self resetStatusElementsWithButtonTitle:nil];
	}
	
	[self setStatusMessage:statusMessage];
}

- (void) resetStatusElementsWithButtonTitle:(NSString *)buttonTitle
{
	[self setUpdateButtonTitle:(buttonTitle ? buttonTitle : @"Check for Updates")];
	[self setUpdateButtonTitleColor:nil];
	[self setProgress:-1.0];
}

- (void) lockGUI:(BOOL)lock
{
	if(lock) {
		updateButton.enabled = NO;
		//projectWebsiteButton.enabled = NO;
		//eponymsDotNetButton.enabled = NO;
		autocheckSwitch.enabled = NO;
		self.navigationItem.rightBarButtonItem.title = @"Abort";
	}
	else {
		updateButton.enabled = YES;
		//projectWebsiteButton.enabled = YES;
		//eponymsDotNetButton.enabled = YES;
		autocheckSwitch.enabled = YES;
		self.navigationItem.rightBarButtonItem.title = @"Done";
	}
}

- (void) updateLabelsWithDateForLastCheck:(NSDate *)lastCheck lastUpdate:(NSDate *)lastUpdate usingEponyms:(NSDate *)usingEponyms
{
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	[dateFormatter setDateStyle:NSDateFormatterShortStyle];
	[dateFormatter setTimeStyle:NSDateFormatterShortStyle];
#if 0	
	// last check
	if(lastCheck) {
		[lastCheckLabel setText:([lastCheck timeIntervalSince1970] > 10.0) ? [dateFormatter stringFromDate:lastCheck] : @"Never"];
	}
	
	// last update
	if(lastUpdate) {
		[lastUpdateLabel setText:([lastUpdate timeIntervalSince1970] > 10.0) ? [dateFormatter stringFromDate:lastUpdate] : @"Never"];
	}
	
	// using eponyms
	//if(usingEponyms) {
		[dateFormatter setTimeStyle:NSDateFormatterNoStyle];
		NSString *usingString = ([usingEponyms timeIntervalSince1970] > 10.0) ? [dateFormatter stringFromDate:usingEponyms] : @"Unknown";
		[usingLabel setText:[NSString stringWithFormat:@"Date: %@", usingString]];
	//}
#endif
	[dateFormatter release];
}


- (void) setUpdateButtonTitle:(NSString *)title
{
	[updateButton setTitle:title forState:(UIControlStateNormal & UIControlStateHighlighted & UIControlStateDisabled & UIControlStateSelected & UIControlStateApplication & UIControlStateReserved)];
}

- (void) setUpdateButtonTitleColor:(UIColor *)color
{
	if(nil == color) {
		color = [UIColor colorWithRed:0.2 green:0.3 blue:0.5 alpha:1.0];		// default button text color
	}
	[updateButton setTitleColor:color forState:(UIControlStateNormal & UIControlStateHighlighted & UIControlStateSelected & UIControlStateDisabled)];
}

- (void) setStatusMessage:(NSString *)message
{
	if(message) {
		// check message length
		CGFloat maxPossibleWidth = progressText.bounds.size.width * 1.1;		// text will be squeezed, so we allow a little overhead
		CGFloat isWidth = [message sizeWithFont:progressText.font].width;
		if(isWidth > maxPossibleWidth) {
			CGFloat fraction = maxPossibleWidth / isWidth;
			NSUInteger useNumChars = roundf([message length] * fraction);
			
			message = [NSString stringWithFormat:@"%@...", [message stringByPaddingToLength:useNumChars withString:@"X" startingAtIndex:0]];
		}
		
		progressText.textColor = [UIColor blackColor];
		progressText.text = message;
	}
	else {
		progressText.textColor = [UIColor grayColor];
		progressText.text = @"CLICK";
	}
}

- (void) setProgress:(CGFloat)progress
{
	if(progress >= 0.0) {
		progressView.hidden = NO;
		progressView.progress = progress;
	}
	else {
		progressView.hidden = YES;
	}
}

- (IBAction) autoCheckSwitchToggled:(id)sender
{
	//UISwitch *mySwitch = sender;
	//[delegate setShouldAutoCheck:mySwitch.on];
}

- (IBAction) alphaCorrectionSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate SetAlphaCorrection:1];
		//[self.parentViewController SetAlphaCorrection:1];
	}
	else {
		[delegate SetAlphaCorrection:0];
	}
}

- (IBAction) threeGSFactorSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate Set3GSFactor:1];
	}
	else {
		[delegate Set3GSFactor:0];
	}}

- (IBAction) loopImageCaptureSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate SetLoopImageCapture:1];
	}
	else {
		[delegate SetLoopImageCapture:0];
	}
}

- (IBAction) showOutlineSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate ShowFeatureOutlines:1];
	}
	else {
		[delegate ShowFeatureOutlines:0];
	}
}

- (IBAction) useCameraARToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate UseCameraAR:1];
	}
	else {
		[delegate UseCameraAR:0];
	}
}


- (IBAction) useQuickestSearchSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate useQuickestSearch:1];
	}
	else {
		[delegate useQuickestSearch:0];
	}
}

- (IBAction) preserveOrientationSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate preserveOrientation:1];
	}
	else {
		[delegate preserveOrientation:0];
	}
}

- (IBAction) disableFDModuleSwitchToggled:(id)sender
{
	UISwitch *mySwitch = sender;
	if(mySwitch.on) {
		[delegate disableFDModule:1];
	}
	else {
		[delegate disableFDModule:0];
	}
}

- (IBAction) maxAttemptsSet:(id)sender
{
	NSInteger val = [maxAttempts.text intValue];
	[delegate maxAttemptsSet:val];
}


-(IBAction) textFieldDoneEditing : (id) sender
{
	
	//[sender resignFirstResponder];
	[delegate maxAttemptsSet:(int)[maxAttempts.text intValue]];
	
}
 
-(IBAction) backgroundTap:(id) sender
{
	
	[maxAttempts resignFirstResponder];
	
}


- (void)addButtonToKeyboard {
	// create custom button
	UIButton *doneButton = [UIButton buttonWithType:UIButtonTypeCustom];
	doneButton.frame = CGRectMake(0, 163, 106, 53);
	doneButton.adjustsImageWhenHighlighted = NO;
	if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 3.0) {
		[doneButton setImage:[UIImage imageNamed:@"DoneUp3.png"] forState:UIControlStateNormal];
		[doneButton setImage:[UIImage imageNamed:@"DoneDown3.png"] forState:UIControlStateHighlighted];
	} else {        
		[doneButton setImage:[UIImage imageNamed:@"DoneUp.png"] forState:UIControlStateNormal];
		[doneButton setImage:[UIImage imageNamed:@"DoneDown.png"] forState:UIControlStateHighlighted];
	}
	[doneButton addTarget:self action:@selector(doneButton:) forControlEvents:UIControlEventTouchUpInside];
	// locate keyboard view
	UIWindow* tempWindow = [[[UIApplication sharedApplication] windows] objectAtIndex:1];
	UIView* keyboard;
	for(int i=0; i<[tempWindow.subviews count]; i++) {
		keyboard = [tempWindow.subviews objectAtIndex:i];
		// keyboard found, add the button
		if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 3.2) {
			if([[keyboard description] hasPrefix:@"<UIPeripheralHost"] == YES)
				[keyboard addSubview:doneButton];
		} else {
			if([[keyboard description] hasPrefix:@"<UIKeyboard"] == YES)
				[keyboard addSubview:doneButton];
		}
	}
}

- (void)keyboardWillShow:(NSNotification *)note {
	// if clause is just an additional precaution, you could also dismiss it
	if ([[[UIDevice currentDevice] systemVersion] floatValue] < 3.2) {
		[self addButtonToKeyboard];
	}
}

- (void)keyboardDidShow:(NSNotification *)note {
	// if clause is just an additional precaution, you could also dismiss it
	if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 3.2) {
		[self addButtonToKeyboard];
    }
}


- (void)doneButton:(id)sender {
	//NSLog(@"doneButton");
    //NSLog(@"Input: %@", maxAttempts.text);
    [maxAttempts resignFirstResponder];
}


#pragma mark -



#pragma mark Updater Delegate
#if 0
- (void) updaterDidStartAction:(EponymUpdater *)updater
{
	[updater retain];
	[self lockGUI:YES];
	[self setStatusMessage:updater.statusMessage];
	[updater release];
}

- (void) updater:(EponymUpdater *)updater didEndActionSuccessful:(BOOL)success
{
	[updater retain];
	[self lockGUI:NO];
	
	if(success) {
		// did check for updates
		if(1 == updater.updateAction) {
			[self newEponymsAreAvailable:updater.newEponymsAvailable];
			[self updateLabelsWithDateForLastCheck:[NSDate date] lastUpdate:nil usingEponyms:nil];
		}
		
		// did update eponyms
		else {
			NSString *statusMessage;
			
			if(updater.numEponymsParsed > 0) {
				statusMessage = [NSString stringWithFormat:@"Created %u eponyms", updater.numEponymsParsed];
				[self updateLabelsWithDateForLastCheck:nil lastUpdate:[NSDate date] usingEponyms:updater.eponymCreationDate];
			}
			else {
				statusMessage = @"No eponyms were created";
			}
			
			[self setStatusMessage:statusMessage];
			[self resetStatusElementsWithButtonTitle:nil];
		}
	}
	
	// an error occurred
	else {
		[self resetStatusElementsWithButtonTitle:@"Try Again"];		
		
		if(updater.downloadFailed && updater.statusMessage) {
			[self alertViewWithTitle:@"Download Failed" message:updater.statusMessage cancelTitle:@"OK"];
		}
		
		[self setStatusMessage:updater.statusMessage];
	}
	
	[updater release];
}

- (void) updater:(EponymUpdater *)updater progress:(CGFloat)progress
{
	[self setProgress:progress];
}
#endif
#pragma mark -



#pragma mark Alert View + Delegate
// alert with one button
- (void) alertViewWithTitle:(NSString *)title message:(NSString *)message cancelTitle:(NSString *)cancelTitle
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:cancelTitle otherButtonTitles:nil];
	[alert show];
	[alert release];
}

// alert with 2 buttons
- (void) alertViewWithTitle:(NSString *)title message:(NSString *)message cancelTitle:(NSString *)cancelTitle otherTitle:(NSString *)otherTitle
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:cancelTitle otherButtonTitles:otherTitle, nil];
	[alert show];
	[alert release];
}

- (void) alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger) buttonIndex
{
	// abort import alert
	if(askingToAbortImport) {
		if(buttonIndex == alertView.firstOtherButtonIndex) {
			//[delegate abortUpdateAction];
			[self dismissMe:nil];
		}
		askingToAbortImport = NO;
	}
	
	// first import alert (can only be accepted at the moment)
	else if(firstTimeLaunch) {
	//	[(eponyms_touchAppDelegate *)delegate loadEponymXMLFromDisk];
	}
}
#pragma mark -



#pragma mark Online Access
- (IBAction) performUpdateAction:(id)sender
{
	//[delegate checkForUpdates:sender];
}

- (void) openWebsite:(NSURL *)url fromButton:(id) button
{
	if(![[UIApplication sharedApplication] openURL:url]) {
		[button setText:@"Failed"];
	}
}

- (IBAction) openProjectWebsite:(id) sender
{
	[self openWebsite:projectWebsiteURL fromButton:sender];
}

- (IBAction) openEponymsDotNet:(id) sender
{
	[self openWebsite:[NSURL URLWithString:@"http://www.eponyms.net/"] fromButton:sender];
}




@end
