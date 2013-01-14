//
//  iAngelEyesPreviewBarMore.h
//  iAngelEyes
//
//  Created by Ashish Lal on 27/05/10.
//  Copyright 2010 NetTech India. All rights reserved.
//
#import <UIKit/UIKit.h>


@class iAngelEyesController;


@interface iAngelEyesPreviewBarMore : UIToolbar {
    iAngelEyesController *delegate;
	
	UIButton *backButton;
	UIButton *browseButton;
	UIButton *colorButton;
	UIButton *saveButton;
}


@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *backButton;
@property (nonatomic, retain) UIButton *browseButton;
@property (nonatomic, retain) UIButton *colorButton;
@property (nonatomic, retain) UIButton *saveButton;

@end
