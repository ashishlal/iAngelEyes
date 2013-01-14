
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "iAngelEyesController.h"

@interface InfoViewController : UIViewController <UIAlertViewDelegate, UIScrollViewDelegate, UITextFieldDelegate> {
	id delegate;
	
	BOOL firstTimeLaunch;
	BOOL askingToAbortImport;
	UIInterfaceOrientation lastInterfaceOrientation;
	
	NSInteger lastCheck;
	NSInteger lastUpdate;
	NSDictionary *infoPlistDict;
	NSURL *projectWebsiteURL;
	
	UISegmentedControl *tabSegments;
	IBOutlet UIView *infoView;
	IBOutlet UIView *updatesView;
	IBOutlet UIView *optionsView;
	IBOutlet UIView *settingsView;
	IBOutlet UIView *helpView;
	//IBOutlet UIImageView *backgroundImage;
	
	
	// Info
	IBOutlet UILabel *versionLabel;
	IBOutlet UILabel *usingLabel;
	IBOutlet UITextView *authorTextView;
	IBOutlet UITextView *propsTextView;
	IBOutlet UIImageView *backgroundImage;
	
	//IBOutlet UIButton *projectWebsiteButton;
	//IBOutlet UIButton *eponymsDotNetButton;
	
	// Updates
	IBOutlet UILabel *lastCheckLabel;
	IBOutlet UILabel *lastUpdateLabel;
	
	IBOutlet UILabel *progressText;
	IBOutlet UIProgressView *progressView;
	
	IBOutlet UIButton *updateButton;
	IBOutlet UISwitch *autocheckSwitch;
	
	// Options
	IBOutlet UISwitch *alphaCorrectionSwitch;
	IBOutlet UISwitch *threeGSFactorSwitch;
	IBOutlet UISwitch *loopImageCaptureSwitch;
	IBOutlet UISwitch *showOutlineSwitch;
	IBOutlet UISwitch *useCameraAR;
	
	//settings
	IBOutlet UISwitch *useQuickestSearchSwitch;
	IBOutlet UISwitch *preserveOrientationSwitch;
	IBOutlet UISwitch *disableFDModuleSwitch;
	IBOutlet UITextField *maxAttempts;
	
	//Help
	IBOutlet UIScrollView *helpScroll;
	IBOutlet UIImageView *helpFile;
}

@property (nonatomic, assign) id delegate;
@property (nonatomic, assign) BOOL firstTimeLaunch;

@property (nonatomic, assign) NSInteger lastCheck;
@property (nonatomic, assign) NSInteger lastUpdate;
@property (nonatomic, retain) NSDictionary *infoPlistDict;
@property (nonatomic, retain) NSURL *projectWebsiteURL;

@property (nonatomic, retain) UISegmentedControl *tabSegments;

@property (nonatomic, retain) IBOutlet UIView *infoView;
@property (nonatomic, retain) IBOutlet UIView *updatesView;
@property (nonatomic, retain) IBOutlet UIView *optionsView;
@property (nonatomic, retain) IBOutlet UIView *settingsView;
@property (nonatomic, retain) IBOutlet UIView *helpView;
@property (nonatomic, retain) IBOutlet UIImageView *backgroundImage;


// Info
@property (nonatomic, retain) IBOutlet UILabel *versionLabel;
@property (nonatomic, retain) IBOutlet UILabel *usingLabel;
@property (nonatomic, retain) IBOutlet UITextView *authorTextView;
@property (nonatomic, retain) IBOutlet UITextView *propsTextView;

//@property (nonatomic, retain) IBOutlet UIButton *projectWebsiteButton;
//@property (nonatomic, retain) IBOutlet UIButton *eponymsDotNetButton;

// Updates
@property (nonatomic, retain) IBOutlet UILabel *lastCheckLabel;
@property (nonatomic, retain) IBOutlet UILabel *lastUpdateLabel;

@property (nonatomic, retain) IBOutlet UILabel *progressText;
@property (nonatomic, retain) IBOutlet UIProgressView *progressView;

@property (nonatomic, retain) IBOutlet UIButton *updateButton;
@property (nonatomic, retain) IBOutlet UISwitch *autocheckSwitch;

// Options
@property (nonatomic, retain) IBOutlet UISwitch *alphaCorrectionSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *threeGSFactorSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *loopImageCaptureSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *showOutlineSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *useCameraAR;

//settings
@property (nonatomic, retain) IBOutlet UISwitch *useQuickestSearchSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *preserveOrientationSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *disableFDModuleSwitch;
@property (nonatomic, retain) IBOutlet UITextField *maxAttempts;

//Help
@property (nonatomic, retain) IBOutlet UIScrollView *helpScroll;
@property (nonatomic, retain) IBOutlet UIImageView *helpFile;

//@property (nonatomic, retain) UIScrollView *scrollView;

- (void) tabChanged:(id)sender;
- (void) updateLabelsWithDateForLastCheck:(NSDate *)lastCheck lastUpdate:(NSDate *)lastUpdate usingEponyms:(NSDate *)usingEponyms;
- (void) dismissMe:(id)sender;

- (void) setUpdateButtonTitle:(NSString *)title;
- (void) setUpdateButtonTitleColor:(UIColor *)color;
- (void) setStatusMessage:(NSString *)message;
- (void) setProgress:(CGFloat) progress;

// Options
- (IBAction) performUpdateAction:(id)sender;
- (IBAction) autoCheckSwitchToggled:(id)sender;
- (IBAction) alphaCorrectionSwitchToggled:(id)sender;
- (IBAction) threeGSFactorSwitchToggled:(id)sender;
- (IBAction) loopImageCaptureSwitchToggled:(id)sender;
- (IBAction) showOutlineSwitchToggled:(id)sender;
- (IBAction) useCameraARToggled:(id)sender;

//settings
- (IBAction) useQuickestSearchSwitchToggled:(id)sender;
- (IBAction) preserveOrientationSwitchToggled:(id)sender;
- (IBAction) disableFDModuleSwitchToggled:(id)sender;
- (IBAction) maxAttemptsSet:(id)sender;

-(IBAction) textFieldDoneEditing : (id) sender;
-(IBAction) backgroundTap:(id) sender;

// Links
//- (IBAction) openProjectWebsite:(id)sender;
//- (IBAction) openEponymsDotNet:(id)sender;

- (void) alertViewWithTitle:(NSString *)title message:(NSString *)message cancelTitle:(NSString *)cancelTitle;
- (void) alertViewWithTitle:(NSString *)title message:(NSString *)message cancelTitle:(NSString *)cancelTitle otherTitle:(NSString *)otherTitle;

@end
