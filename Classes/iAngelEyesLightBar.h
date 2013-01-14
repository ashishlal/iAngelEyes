//
//  iAngelEyesLightBar.h
//  iAngelEyes
//
//  Created by Ashish Lal on 17/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import <UIKit/UIKit.h>

@class iAngelEyesController;


@interface iAngelEyesLightBar : UIToolbar {
	iAngelEyesController *delegate;
	
	UIButton *backButton;
	UIButton *L0Button;
	UIButton *L1Button;
	UIButton *L2Button;
	UIButton *L3Button;
	UIButton *L4Button;
}

@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *backButton;
@property (nonatomic, retain) UIButton *L0Button;
@property (nonatomic, retain) UIButton *L1Button;
@property (nonatomic, retain) UIButton *L2Button;
@property (nonatomic, retain) UIButton *L3Button;
@property (nonatomic, retain) UIButton *L4Button;

@end

