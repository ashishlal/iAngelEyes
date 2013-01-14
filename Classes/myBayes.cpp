/*
 *  myBayes.cpp
 *  PictureMe
 *
 *  Created by Ashish Lal on 07/03/10.
 *  Copyright 2010 NetTech India. All rights reserved.
 *
 */

#include "cv.h"
#include "highgui.h"
#include "ml.h"

 #ifdef __cplusplus
extern "C" {
#endif
	
enum{FACE=0, NOSE, MOUTH, EYE, LEYE, REYE, BG};
//void populateFeatureData(CvMat* train, CvMat* responses, int iFeatureType, int size );
	
void myInitNormalBayesClassifier();

#ifdef __cplusplus
}
#endif
		
class CvNormalBayesClassifier;
CvNormalBayesClassifier *bayes;

void myInitNormalBayesClassifier1()
{
	bayes = new CvNormalBayesClassifier();
}