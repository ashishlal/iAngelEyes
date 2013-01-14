//
//  iAngelEyesPreviewBarCV.h
//  iAngelEyes
//
//  Created by Ashish Lal on 06/07/10.
//  Copyright 2010 NetTech India. All rights reserved.
//

#import <UIKit/UIKit.h>


@class iAngelEyesController;


@interface iAngelEyesPreviewBarCV : UIToolbar {
    iAngelEyesController *delegate;
	
	UIButton *moveButton;
	UIButton *rotateButton;
	UIButton *resizeButton;
	UIButton *forwardButton;
    
}


@property (nonatomic, assign) iAngelEyesController *delegate;
@property (nonatomic, retain) UIButton *moveButton;
@property (nonatomic, retain) UIButton *rotateButton;
@property (nonatomic, retain) UIButton *resizeButton;
@property (nonatomic, retain) UIButton *forwardButton;

@end
