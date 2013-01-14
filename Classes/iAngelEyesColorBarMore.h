//
//  iAngelEyesColorBarMore.h
//  iAngelEyes
//
//  Created by Ashish Lal on 04/06/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import <UIKit/UIKit.h>

@class iAngelEyesController;

@interface iAngelEyesColorBarMore : UIToolbar {
	iAngelEyesController *delegate;
	
	UIButton *backButton;
	UIButton *specularButton;
	UIButton *shininessButton;
	//UIButton *lightButton;
}

@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *backButton;
@property (nonatomic, retain) UIButton *specularButton;
@property (nonatomic, retain) UIButton *shininessButton;
//@property (nonatomic, retain) UIButton *lightButton;

@end
