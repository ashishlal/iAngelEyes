//
//  iAngelEyesBrowseBar.h
//  iAngelEyes
//
//  Created by Ashish Lal on 01/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import <UIKit/UIKit.h>


@class iAngelEyesController;


@interface iAngelEyesBrowseBar : UIToolbar {
    iAngelEyesController *delegate;
    
    UIButton *backButton;
	UIButton *firstButton;
	UIButton *prevButton;
	UIButton *nextButton;
	UIButton *lastButton;
}


@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *backButton;
@property (nonatomic, retain) UIButton *firstButton;
@property (nonatomic, retain) UIButton *prevButton;
@property (nonatomic, retain) UIButton *nextButton;
@property (nonatomic, retain) UIButton *lastButton;

@end
