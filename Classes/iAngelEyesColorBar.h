//
//  iAngelEyesColorBar.h
//  iAngelEyes
//
//  Created by Ashish Lal on 02/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import <UIKit/UIKit.h>

@class iAngelEyesController;

@interface iAngelEyesColorBar : UIToolbar {
	iAngelEyesController *delegate;
	
	UIButton *backButton;
	UIButton *textureButton;
	UIButton *ambienceButton;
	UIButton *diffusionButton;
	UIButton *forwardButton;
}

@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *backButton;
@property (nonatomic, retain) UIButton *textureButton;
@property (nonatomic, retain) UIButton *ambienceButton;
@property (nonatomic, retain) UIButton *diffusionButton;
@property (nonatomic, retain) UIButton *forwardButton;

@end
