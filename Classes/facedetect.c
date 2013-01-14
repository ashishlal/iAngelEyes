#define CV_NO_BACKWARD_COMPATIBILITY

#include "cv.h"
//#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>

#include "ConstantsAndMacros.h"

static CvMemStorage* storage = NULL;
static CvHaarClassifierCascade* cascade = NULL;

CvHaarClassifierCascade* face_cascade = NULL;
CvHaarClassifierCascade* profile_face_cascade = NULL;
//static CvHaarClassifierCascade* face_cascade = NULL;
//static CvHaarClassifierCascade* profile_face_cascade=NULL;
static CvHaarClassifierCascade* nose_cascade = NULL;
static CvHaarClassifierCascade* mouth_cascade = NULL;
static CvHaarClassifierCascade* leye_cascade = NULL;
static CvHaarClassifierCascade* reye_cascade = NULL;

//static CvHaarClassifierCascade* nested_cascade = 0;

int use_nested_cascade = 0;
enum{FACE=0, NOSE, MOUTH, EYE, LEYE, REYE, PROFILE_FACE, BG};
extern CvSeq* mycvHaarDetectObjects( const CvArr* _img,
					  CvHaarClassifierCascade* cascade,
					  CvMemStorage* storage, double scale_factor,
					  int min_neighbors, int flags, CvSize min_size );
extern void myInitNormalBayesClassifier();
void detect_and_draw( IplImage* img, CvRect* found_face, CvSize sz );
int detect_multiple1( IplImage* img, CvRect* found_face[], CvSize sz, int feature  );
void detect_multiple( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
					CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], 
					 CvRect *unscaled_found_eyes[], CvSize sz, int *);
void detect_profileface_multiple( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], CvRect *found_eyes[], int *tot, 
					 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], CvRect* unscaled_found_mouth[], 
					 CvRect *unscaled_found_eyes[], CvSize sz, int *);
void detect_nose(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_nose[], 
				 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], int *tot, int fN, CvSize	sz, int *);
void detect_mouth(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_nose[], 
				 CvRect* unscaled_found_face[], CvRect* unscaled_found_mouth[], CvRect* unscaled_found_nose[],
				  int *tot, int totN, int fN, CvSize	sz);
void detect_eyes(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_nose[], 
				 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], int *tot, int fN, CvSize	sz, int *eyes_det);
double scale = 1.5;
double g_eye_mid_pt_x = 0.0;
double g_eye_mid_pt_y = 0.0;
double g_nose_mid_pt_x_iphone = 0.0, g_nose_mid_pt_y_iphone = 0.0;

int g_focus_on_image=0;
const double IPHONEW=320.0f;
const double IPHONEH=480.0f;
double g_using_scaled_image = 0.0f;
int use_quickest_search = 0;

//IplImage *gray = 0, *small_img = 0;

typedef unsigned char mybool;
enum _my_e {
	myfalse=0, 
	mytrue
};



void init_detection(char* cascade_location) {
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_location, 0, 0, 0 );
	storage = cvCreateMemStorage(0);
}



void reinit_detection(char* cascade_location) {
	if (cascade)
    {
        cvReleaseHaarClassifierCascade(&cascade);
    }	
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_location, 0, 0, 0 );
}



void release_detection() {
	if (storage)
    {
        cvReleaseMemStorage(&storage);
    }
    if (cascade)
    {
        cvReleaseHaarClassifierCascade(&cascade);
    }
	//cvReleaseImage(&gray);
	//cvReleaseImage(&small_img);
}

void init_detection_multiple(char* cascade_location, char* loc1, char* loc2, char* loc3, char *loc4, 
							 char *loc5, int flag, int searchFlag) 
{
	if(flag & INIT_FACE_CASCADE) 
		face_cascade = (CvHaarClassifierCascade*)cvLoad( cascade_location, 0, 0, 0 );
	if(flag & INIT_PROFILE_FACE_CASCADE)
		profile_face_cascade = (CvHaarClassifierCascade*)cvLoad( loc5, 0, 0, 0 );
	//profile_face_cascade = cvLoadHaarClassifierCascade(loc5, cvSize(20,20));
	if(flag & INIT_NOSE_CASCADE) 
		nose_cascade = (CvHaarClassifierCascade*)cvLoad( loc1, 0, 0, 0 );
#if 0
	if(flag & INIT_MOUTH_CASCADE)
		mouth_cascade = (CvHaarClassifierCascade*)cvLoad( loc2, 0, 0, 0 );
#else
	mouth_cascade = NULL;
#endif
	if(flag & INIT_LEYE_CASCADE)
		leye_cascade = (CvHaarClassifierCascade*)cvLoad( loc3, 0, 0, 0 );
	if(flag &INIT_REYE_CASCADE)
		reye_cascade = (CvHaarClassifierCascade*)cvLoad( loc4, 0, 0, 0 );
	
	use_quickest_search = searchFlag;
	cascade=face_cascade;
	storage = cvCreateMemStorage(0);
	
	//profile_face_cascade = cvLoadHaarClassifierCascade(loc5, cvSize(24,24));
    /* images are assigned inside cvHaarDetectObject, so pass NULL pointers here */
   // CvHidHaarClassifierCascade* hid_cascade = cvCreateHidHaarClassifierCascade( profile_face_cascade, 0, 0, 0, 1 );
    /* the original cascade is not needed anymore */
    //cvReleaseHaarClassifierCascade( &profile_face_cascade );
	//profile_face_cascade = hid_cascade;
	
}
void release_detection_multiple() {
	if (storage)
    {
        cvReleaseMemStorage(&storage);
    }
    if (face_cascade)
    {
        cvReleaseHaarClassifierCascade(&face_cascade);
    }
	if (nose_cascade)
    {
        cvReleaseHaarClassifierCascade(&nose_cascade);
    }
	if (mouth_cascade)
    {
        cvReleaseHaarClassifierCascade(&mouth_cascade);
    }
	if (leye_cascade)
    {
        cvReleaseHaarClassifierCascade(&leye_cascade);
    }
	if (reye_cascade)
    {
        cvReleaseHaarClassifierCascade(&reye_cascade);
    }
	if (profile_face_cascade)
    {
        cvReleaseHaarClassifierCascade(&profile_face_cascade);
    }
	//if(gray) cvReleaseImage(&gray);
	//if(small_img) cvReleaseImage(&small_img);
}


#define PAD_FACE 40
#define PAD_FACE_2 80

#if 0
void detect_and_draw( IplImage* img, CvRect* found_face, CvSize sz )
{
	static CvRect prev;
	
	if(!gray) {
		gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
		small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
							 cvRound (img->height/scale)), 8, 1 );
	}

	if(prev.width > 0 && prev.height > 0) {
		cvSetImageROI(small_img, prev);

		CvRect tPrev = cvRect(prev.x * scale, prev.y * scale, prev.width * scale, prev.height * scale);
		cvSetImageROI(img, tPrev);
		cvSetImageROI(gray, tPrev);
	} else {
		cvResetImageROI(img);
		cvResetImageROI(small_img);
		cvResetImageROI(gray);
	}
	
    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );

//	for(i=0;i<10;i++) {
//		double t = (double)cvGetTickCount();
		CvSeq* faces = mycvHaarDetectObjects( small_img, cascade, storage,
										   1.2, 0, 0
										   |CV_HAAR_FIND_BIGGEST_OBJECT
										   |CV_HAAR_DO_ROUGH_SEARCH
										   //|CV_HAAR_DO_CANNY_PRUNING
										   //|CV_HAAR_SCALE_IMAGE
											 ,sz);
										   //cvSize(30, 30) );
//		t = (double)cvGetTickCount() - t;
//		printf( "detection time = %gms, faces: %d\n", t/((double)cvGetTickFrequency()*1000.), faces->total );
		
	if(faces->total>0) {
		CvRect* r = (CvRect*)cvGetSeqElem( faces, 0 );
		int startX,startY;
		if(prev.width > 0 && prev.height > 0) {
			r->x += prev.x;
			r->y += prev.y;
		}
		startX = MAX(r->x - PAD_FACE,0);
		startY = MAX(r->y - PAD_FACE,0);
		int w = small_img->width - startX - r->width - PAD_FACE_2;
		int h = small_img->height - startY - r->height - PAD_FACE_2;
		int sw = r->x - PAD_FACE, sh = r->y - PAD_FACE;
		prev = cvRect(startX, startY, 
					  r->width + PAD_FACE_2 + ((w < 0) ? w : 0) + ((sw < 0) ? sw : 0),
					  r->height + PAD_FACE_2 + ((h < 0) ? h : 0) + ((sh < 0) ? sh : 0));
//		printf("found face (%d,%d,%d,%d) setting ROI to (%d,%d,%d,%d)\n",r->x,r->y,r->width,r->height,prev.x,prev.y,prev.width,prev.height);
		found_face->x = (int)((double)r->x * scale);
		found_face->y = (int)((double)r->y * scale);
		found_face->width = (int)((double)r->width * scale);
		found_face->height = (int)((double)r->height * scale);
	} else {
		prev.width = prev.height = found_face->width = found_face->height = 0;
	}
}
#endif

#ifndef MAX_RECT
#define MAX_RECT 5
#endif

int detect_multiple1( IplImage* img, CvRect* found_obj[], CvSize sz, int feature  )
{

	static CvRect face_rect;
	IplImage *gray=NULL, *small_img=NULL;
	int tot=0;
	
	switch (feature) {
		case FACE:
			cascade=face_cascade;
			break;
		case NOSE:
			cascade=nose_cascade;
			break;
		case MOUTH:
			cascade=mouth_cascade;
			break;
		case EYE:
		case LEYE:
			cascade=leye_cascade;
			break;
		case REYE:
			cascade=reye_cascade;
			break;
		case PROFILE_FACE:
			cascade=profile_face_cascade;
			break;
		default:
			cascade = face_cascade;
			break;
	}	
	
	if (feature == FACE) {
		//cvResetImageROI(img);
		if(gray) {
			cvResetImageROI(gray);
			cvReleaseImage(&gray);
		}
		if(small_img) {
			cvResetImageROI(small_img);
			cvReleaseImage(&small_img);
		}
		gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
		small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
										  cvRound (img->height/scale)), 8, 1 );
		
		cvCvtColor( img, gray, CV_BGR2GRAY );
		cvResize( gray, small_img, CV_INTER_LINEAR );
		cvEqualizeHist( small_img, small_img );
	}

	else{
		cvSetImageROI(small_img, face_rect);
		//		printf("found face (%d,%d,%d,%d) setting ROI to (%d,%d,%d,%d)\n",r->x,r->y,r->width,r->height,prev.x,prev.y,prev.width,prev.height);
	}	
		
	cvClearMemStorage( storage );
	
	//	for(i=0;i<10;i++) {
	//		double t = (double)cvGetTickCount();
	
	CvSeq* objects = cvHaarDetectObjects( small_img, cascade, storage,
										 1.1, 2, 0
									   //1.2, 0, 0
										// |CV_HAAR_FIND_BIGGEST_OBJECT
										// |CV_HAAR_DO_ROUGH_SEARCH
										// |CV_HAAR_DO_CANNY_PRUNING
										// |CV_HAAR_SCALE_IMAGE
										 ,
										 sz);
										 //cvSize(30, 30) );
	//		t = (double)cvGetTickCount() - t;
	//		printf( "detection time = %gms, faces: %d\n", t/((double)cvGetTickFrequency()*1000.), faces->total );
	tot=objects->total;
	int i=0;
	if(objects->total>0) {
		if(tot > MAX_RECT) tot=MAX_RECT;
		for(i=0; ((i < tot) &&( i < MAX_RECT)); i++) {
			CvRect* r = (CvRect*)cvGetSeqElem( objects, i );
			
			if(feature == FACE) { 
				if(i==0)
					face_rect = cvRect(r->x, r->y, r->width, r->height);
			}
			
			else {
				if(face_rect.width > 0 && face_rect.height > 0) {
					r->x += face_rect.x;
					r->y += face_rect.y;
				}
				
			}
			
			found_obj[i]->x = (int)((double)r->x * scale);
			found_obj[i]->y = (int)((double)r->y * scale);
			found_obj[i]->width = (int)((double)r->width * scale);
			found_obj[i]->height = (int)((double)r->height);
			if(feature == FACE) 
				found_obj[i]->height = (int)((double)r->height * scale);
			
		}
	} 
	
	if(feature != FACE)
		cvResetImageROI(small_img);
	// eyes are last
	if(feature == EYE) {
		cvReleaseImage(&gray);
		cvReleaseImage(&small_img);
	}
	return tot;
}

void detect_nose(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_nose[], 
				 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], int *tot, int fN, CvSize	sz, int *nose_det)
{
	
	CvSeq* noses=NULL;
	int faceX, faceY, faceWidth, faceHeight;
	CvRect *r = unscaled_found_face[fN];
	//int faceMidy; 
	int faceMidx;
	faceX = r->x;
	faceY = r->y;
	faceWidth = r->width;
	faceHeight = r->height;
	faceMidx = faceX + (faceWidth/2);
	//faceMidy = faceY + (faceHeight/2);
	int min_wt=0, wt=0;
	int min_wt_index=0;
	//for(int v=0; v < 10; v++) wt[v]=0;
	
	int currX, currY, currWidth, currHeight;
	int currMidy, currMidx;
	
	if( !nose_cascade ) return;
	int n=0;
	
	noses = cvHaarDetectObjects(small_img_roi, nose_cascade, storage,
								1.1, 2, 0
								//|CV_HAAR_FIND_BIGGEST_OBJECT
								//|CV_HAAR_DO_ROUGH_SEARCH
								//|CV_HAAR_DO_CANNY_PRUNING
								//|CV_HAAR_SCALE_IMAGE
								, cvSize(0, 0) );
	
	int currDelta=0;
	int prevDelta=10000;
	for(int j = 0; j < (noses ? noses->total : 0); j++ )
	{
		
		CvRect* nr = (CvRect*)cvGetSeqElem( noses, j );
		
		if(g_using_scaled_image) {
			nr->x = (int)(((double)(nr->x)) / g_using_scaled_image);
			nr->y = (int)(((double)(nr->y)) / g_using_scaled_image);
			nr->width = (int)(((double)(nr->width)) / g_using_scaled_image);
			nr->height = (int)(((double)(nr->height)) / g_using_scaled_image);
		
			if(g_using_scaled_image == 0.75f) {
				double myNum = (55.0f * g_using_scaled_image);
			
				nr->x += (int)(myNum);
				nr->y += (int)(myNum);
				nr->width += (int)(myNum);
				nr->height += (int)(myNum);
			}
		}
		
		currX = (r->x + nr->x);
		currY = (r->y + nr->y);
		currWidth = nr->width;
		currHeight = nr->height;
		
		
		
		//if(noses->total > 1) {
		if((currX > (faceX + faceWidth)) || (currY > (faceY + faceHeight))) continue;
		if((currX < faceX) || (currWidth > faceWidth) || (currHeight > faceHeight)) continue;
		//}
		currMidx = currX + (currWidth/2);
		currMidy = currY + (currHeight/2);
#if DEBUG_FALSE_POSITIVES
//#if 1
		
		
		printf("n: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
		printf("n: %d %d %d %d\n", nr->x, nr->y, nr->width, nr->height);
		
		printf("n: (%d %d) (%d %d) (%d %d) (%d %d) (%d %d)\n", 
			   currMidy, faceX, currMidx, (faceX + faceWidth),
			   currMidx, faceX,
			   currMidy, (faceY + (int)((float)faceHeight * 0.2)),
			   currMidy, (faceY + (int)((float)faceHeight * 0.55))
			   );
#endif
		currDelta = (currMidx > faceMidx)?(currMidx - faceMidx):(faceMidx - currMidx);
		if(((currMidx > faceX) && 
		   (currMidx < (faceX + faceWidth)) 
		 //  (currY > (faceY + (int)((float)faceHeight * 0.2))) &&
		   && (currDelta < prevDelta)
		  && (currY <= (faceY + (int)((float)faceHeight * 0.55)))) 
		 // || (noses->total == 1)
		   ) 
		{
			
			double myscale=scale; /* IMP: double! */
			//double eye_mid_pt_x_iphone = 0.0;
			double eye_mid_pt_y_iphone = 0.0;
			double nose_mid_pt_x = 0.0, nose_mid_pt_y = 0.0, nose_y=0.0;
			double nose_mid_pt_x_iphone = 0.0, nose_mid_pt_y_iphone = 0.0, nose_y_iphone=0.0;
			int testIdx = MAX_RECT-1;
			
			found_nose[testIdx]->x = (int)((double)(r->x + nr->x) * myscale);
			found_nose[testIdx]->y = (int)((double)(r->y + nr->y) * myscale);
			found_nose[testIdx]->width = (int)((double)nr->width * 1.5);
			found_nose[testIdx]->height = (int)((double)nr->height); 
			
			nose_mid_pt_x = ((double)(found_nose[testIdx]->x) + (double)(found_nose[testIdx]->width/2));
			nose_mid_pt_y = ((double)(found_nose[testIdx]->y) + (double)(found_nose[testIdx]->height/2));
			nose_y = (double)(found_nose[testIdx]->y);
			
			// iphone co-ords
			nose_mid_pt_x_iphone = IPHONEW - nose_mid_pt_x;
			nose_mid_pt_y_iphone = IPHONEH - nose_mid_pt_y;
			nose_y_iphone = IPHONEH - nose_y;
			
			//eye_mid_pt_x_iphone = IPHONEW - g_eye_mid_pt_x;
			eye_mid_pt_y_iphone = IPHONEH - g_eye_mid_pt_y;
			
			if(nose_mid_pt_y_iphone >= eye_mid_pt_y_iphone) 
			{
				wt = (nose_mid_pt_y_iphone - eye_mid_pt_y_iphone);
				if(wt < min_wt) {
					min_wt = wt;
					min_wt_index = j;
				}
				//if(noses->total > 1)
					continue;
			}
			if(nose_y_iphone >= eye_mid_pt_y_iphone) 
			{
				wt = (nose_y_iphone - eye_mid_pt_y_iphone);
				if(wt < min_wt) {
					min_wt = wt;
					min_wt_index = j;
				}
				//if(noses->total > 1)
					continue;
			}
			
			found_nose[n]->x = (int)((double)(r->x + nr->x) * myscale);
			found_nose[n]->y = (int)((double)(g_eye_mid_pt_y) * 1.0);
			//found_nose[n]->y = (int)((double)(r->y + nr->y) * myscale);
			found_nose[n]->width = (int)((double)nr->width * 1.75);
			
			
			// take the full nose
			if(g_focus_on_image) {
				found_nose[n]->height = (int)(((double)nr->height) * 1.25f); 
				found_nose[n]->height += (int)(((double)(eye_mid_pt_y_iphone - nose_y_iphone))* 1.25f);
			}
			else {
				found_nose[n]->height = (int)(nr->height); 
				found_nose[n]->height += (int)(eye_mid_pt_y_iphone - nose_y_iphone);
			}

			unscaled_found_nose[n]->x = nr->x ;
			unscaled_found_nose[n]->y = nr->y ;
			unscaled_found_nose[n]->width = nr->width ;
			unscaled_found_nose[n]->height = nr->height;
			
			prevDelta=currDelta;
			
			n++;
			if(n==1) {
				g_nose_mid_pt_x_iphone = nose_mid_pt_x_iphone;
				g_nose_mid_pt_y_iphone = nose_mid_pt_y_iphone;
			}
			
			if(nose_mid_pt_x != g_eye_mid_pt_x) {
			
				found_nose[n]->x = (int)((double)(found_nose[n]->x) + (nose_mid_pt_x - g_eye_mid_pt_x));
			}
		}
		
	}
	*nose_det=1;
	if((n == 0) && (noses) && (noses->total > 0))
	{
		/* nose not found */		
		CvRect* nr = (CvRect*)cvGetSeqElem( noses, min_wt_index );
		
		double myscale=scale; /* IMP: double! */
		//double eye_mid_pt_x_iphone = 0.0;
		double eye_mid_pt_y_iphone = 0.0;
		double nose_mid_pt_x = 0.0, nose_mid_pt_y = 0.0, nose_y=0.0;
		double nose_mid_pt_x_iphone = 0.0, nose_mid_pt_y_iphone = 0.0, nose_y_iphone=0.0;
		int testIdx = MAX_RECT-1;
		
		*nose_det = 0;
		found_nose[testIdx]->x = (int)((double)(r->x + nr->x) * myscale);
		found_nose[testIdx]->y = (int)((double)(r->y + nr->y) * myscale);
		found_nose[testIdx]->width = (int)((double)nr->width * 1.5);
		found_nose[testIdx]->height = (int)((double)nr->height); 
			
		nose_mid_pt_x = ((double)(found_nose[testIdx]->x) + (double)(found_nose[testIdx]->width/2));
		nose_mid_pt_y = ((double)(found_nose[testIdx]->y) + (double)(found_nose[testIdx]->height/2));
		nose_y = (double)(found_nose[testIdx]->y);
			
		// iphone co-ords
		nose_mid_pt_x_iphone = IPHONEW - nose_mid_pt_x;
		nose_mid_pt_y_iphone = IPHONEH - nose_mid_pt_y;
		nose_y_iphone = IPHONEH - nose_y;
			
		//eye_mid_pt_x_iphone = IPHONEW - g_eye_mid_pt_x;
		eye_mid_pt_y_iphone = IPHONEH - g_eye_mid_pt_y;
			
					
		found_nose[n]->x = (int)((double)(r->x + nr->x) * myscale);
		found_nose[n]->y = (int)((double)(g_eye_mid_pt_y) * 1.0);
		//found_nose[n]->y = (int)((double)(r->y + nr->y) * myscale);
		found_nose[n]->width = (int)((double)nr->width * 2.0);
			
			
		// take the full nose
		if(g_focus_on_image) {
			found_nose[n]->height = (int)(((double)nr->height) * 1.25f); 
			found_nose[n]->height += (int)(((double)(eye_mid_pt_y_iphone - nose_y_iphone))* 1.25f);
		}
		else {
			found_nose[n]->height = (int)(nr->height); 
			if(eye_mid_pt_y_iphone > nose_y_iphone) {
				found_nose[n]->height += (int)(eye_mid_pt_y_iphone - nose_y_iphone);
			}
			else {
				found_nose[n]->height += (int)(nose_y_iphone - eye_mid_pt_y_iphone);
			}
		}
			
		unscaled_found_nose[n]->x = nr->x ;
		unscaled_found_nose[n]->y = nr->y ;
		unscaled_found_nose[n]->width = nr->width ;
		unscaled_found_nose[n]->height = nr->height;
			
		//prevDelta=currDelta;
			
		n++;
		if(n==1) {
			g_nose_mid_pt_x_iphone = nose_mid_pt_x_iphone;
			g_nose_mid_pt_y_iphone = nose_mid_pt_y_iphone;
		}
			
		if(nose_mid_pt_x != g_eye_mid_pt_x) {
				
			found_nose[n]->x = (int)((double)(found_nose[n]->x) + (nose_mid_pt_x - g_eye_mid_pt_x));
		}
	
	}
	*tot=0;
	//if((noses->total) && (n >0)) *tot=((n <MAX_RECT) ? 1:MAX_RECT);
	if((noses) && (noses->total) && (n >0)) 
	{
		*tot=((n <MAX_RECT) ? n:MAX_RECT);
#if 0	
		for(int l=0; l < (*tot); l++) {
		//printf("e: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
			printf("n%d: %d %d %d %d\n", l, (int)found_nose[l]->x, (int)found_nose[l]->y, found_nose[l]->width, 
				   found_nose[l]->height);
		}
#endif
	}
	return;
}


void detect_mouth(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_mouth[], 
				 CvRect* unscaled_found_face[], CvRect* unscaled_found_mouth[], CvRect* unscaled_found_nose[],
				  int *tot, int totN, int fN, CvSize	sz)
{
	CvSeq* mouths;
	int faceX, faceY, faceWidth, faceHeight;
	CvRect *r = unscaled_found_face[fN];
	//int faceMidy;
	int faceMidx;
	faceX = r->x;
	faceY = r->y;
	faceWidth = r->width;
	faceHeight = r->height;
	
	//faceMidy = faceY + (faceHeight/2);
	faceMidx = faceX + (faceWidth/2);
	int currX, currY, currWidth, currHeight;
	int currMidy, currMidx;
	//int noseMidy=0;
	
	
	if( !mouth_cascade ) return;
	int n=0;
	mouths = cvHaarDetectObjects( small_img_roi, mouth_cascade, storage,
								 1.1, 2, 0
								 //|CV_HAAR_FIND_BIGGEST_OBJECT
								 //|CV_HAAR_DO_ROUGH_SEARCH
								 //|CV_HAAR_DO_CANNY_PRUNING
								 //|CV_HAAR_SCALE_IMAGE
								 , cvSize(0, 0) );
	
	int currDelta=0, currDelta1=0, currDelta2=0, currDelta3=0, currDelta4=0;
	int prevDelta=10000;
	for( int j = 0; j < (mouths ? mouths->total : 0); j++ )
	{
		CvRect* mr = (CvRect*)cvGetSeqElem( mouths, j );
		if(g_using_scaled_image) {
			mr->x = (int)(((double)(mr->x)) / g_using_scaled_image);
			mr->y = (int)(((double)(mr->y)) / g_using_scaled_image);
			mr->width = (int)(((double)(mr->width)) / g_using_scaled_image);
			mr->height = (int)(((double)(mr->height)) / g_using_scaled_image);
			
			if(g_using_scaled_image == 0.75f) {
				double myNum = (55.0f * g_using_scaled_image);
				
				mr->x += (int)(myNum);
				mr->y += (int)(myNum);
				mr->width += (int)(myNum);
				mr->height += (int)(myNum);
			}
		}
		currX = (r->x + mr->x);
		currY = (r->y + mr->y);
		currWidth = mr->width;
		currHeight = mr->height;
		
		currMidy = currY + (currHeight/2);
		currMidx = currX + (currWidth/2);
		
		if((currX > (faceX + faceWidth)) || (currY > (faceY + faceHeight))) continue;
		if((currX < faceX) || (currWidth > faceWidth) || (currHeight > faceHeight)) continue;
//#if DEBUG_FALSE_POSITIVES
#if 0
		printf("m: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
		printf("m: %d %d %d %d\n", mr->x, mr->y, mr->width, mr->height);
		
		printf("m: (%d %d) (%d %d) (%d %d) (%d %d) (%d %d)\n", 
			   currMidx, faceX, currMidx, (faceX + faceWidth),
			   currMidx, faceX,
			   currY, (int)((faceY + faceHeight) * 0.5),
			   currY, (int)((faceY + faceHeight) * 0.84)
			   );
#endif				
		double myscale=scale;
		if((currMidx > faceX) 
		   && (currMidx < (faceX + faceWidth))
		   
		   //&& (currY >= (int)((faceY + faceHeight) * 0.5)) /* take it above since we subtract later on */
		  // && (currY <= (int)((faceY + faceHeight) * 0.5))
		 //  (currY >= (int)((faceY + faceHeight) * 0.71)) &&
		 //  (currY <= (int)((faceY + faceHeight) * 0.84)) 
		   ) 
		{
			//int totNoses=totN;
			//int idx= (j < totNoses)?0:(j-1);
			int idx=0;
			int wt1=2, wt2=2, wt3=3;
			int noseMidx=0, noseMidy=0;
			
			
			
			
			if(totN) 
			///if(0)
			
			{
				noseMidx = (int)((unscaled_found_nose[idx]->x + r->x) + (unscaled_found_nose[idx]->width/2));
				noseMidy = (int)((unscaled_found_nose[idx]->y + r->y) + (unscaled_found_nose[idx]->height/2));
				
				currDelta1 = (unscaled_found_nose[idx]->width - currWidth);
				
				currDelta2 = ((currMidx >= noseMidx)? (currMidx - noseMidx):(noseMidx-currMidx));
				currDelta3 = (currMidx > faceMidx)?(currMidx - faceMidx):(faceMidx - currMidx);
				currDelta4 = (currMidy - noseMidy);
#if 0				
				printf("m: %d %d %d %d\n", unscaled_found_nose[idx]->x, unscaled_found_nose[idx]->y, 
					   unscaled_found_nose[idx]->width, unscaled_found_nose[idx]->height);
#endif
			}
			else 
			{
				currDelta3 = (currMidx > faceMidx)?(currMidx - faceMidx):(faceMidx - currMidx);
			}
			currDelta = (currDelta1 * wt1) + (currDelta2 * wt2) + (currDelta3 * wt3) + (currDelta4 * wt1);
#if 0
			
			printf("m: %d %d %d %d %d %d %d\n", noseMidx, noseMidy, currDelta1, currDelta2, currDelta3, currDelta4, currDelta);
			
#endif		
			// take the closest to the center of the face
			if(currDelta < prevDelta) 
			{
				//int m=n;
				int m=0;
				double mouth_mid_pt_x = 0.0, mouth_mid_pt_y = 0.0;
				double mouth_mid_pt_x_iphone = 0.0, mouth_mid_pt_y_iphone = 0.0;
				double face_mid_pt_x_iphone = 0.0, face_mid_pt_y_iphone = 0.0;
				int testIdx=MAX_RECT-1;
				
				found_mouth[testIdx]->x = (int)((double)(r->x + mr->x) * myscale);
				found_mouth[testIdx]->y = (int)((double)(r->y + mr->y) * myscale);
				found_mouth[testIdx]->width = (int)((double)(mr->width) * 1.5 );
				found_mouth[testIdx]->height = (int)((double)mr->height); 
				
				mouth_mid_pt_x = ((double)(found_mouth[testIdx]->x) + (double)(found_mouth[testIdx]->width/2));
				mouth_mid_pt_y = ((double)(found_mouth[testIdx]->y) + (double)(found_mouth[testIdx]->height/2));
				
				// iphone co-ords
				mouth_mid_pt_x_iphone = IPHONEW - mouth_mid_pt_x;
				mouth_mid_pt_y_iphone = IPHONEH - mouth_mid_pt_y;
				
				face_mid_pt_x_iphone = IPHONEW - (r->x * myscale) - (r->width * myscale/2) ;
				face_mid_pt_y_iphone = IPHONEH - (r->y * myscale) - (r->height * myscale/2);
				
				printf("--------This?-------: %.0f %.0f %.0f %.0f %.0f %.0f\n\n\n",
					   mouth_mid_pt_x_iphone, face_mid_pt_x_iphone, g_nose_mid_pt_x_iphone,
					   mouth_mid_pt_y_iphone, face_mid_pt_y_iphone, g_nose_mid_pt_y_iphone);
				
				if(mouth_mid_pt_y_iphone >= face_mid_pt_y_iphone) continue;
				if(mouth_mid_pt_y_iphone >= g_nose_mid_pt_y_iphone) continue;
				
				
				printf("--------Y-This-------: %.0f %.0f %.0f %.0f %.0f %.0f\n\n\n",
					   mouth_mid_pt_x_iphone, face_mid_pt_x_iphone, g_nose_mid_pt_x_iphone,
					   mouth_mid_pt_y_iphone, face_mid_pt_y_iphone, g_nose_mid_pt_y_iphone);
				
				
				found_mouth[m]->x = (int)((double)(r->x + mr->x) * myscale);
				found_mouth[m]->y = (int)((double)(r->y + mr->y) * myscale);
				found_mouth[m]->width = (int)((double)(mr->width) * 1.5 );
				found_mouth[m]->height = (int)((double)mr->height); 
				
				unscaled_found_mouth[m]->x = mr->x;
				unscaled_found_mouth[m]->y = mr->y;
				unscaled_found_mouth[m]->width = mr->width;
				unscaled_found_mouth[m]->height = mr->height; 
				
				if(g_nose_mid_pt_x_iphone != mouth_mid_pt_x_iphone) {
					found_mouth[n]->x = (int)((double)(found_mouth[n]->x) + (mouth_mid_pt_x_iphone - g_nose_mid_pt_x_iphone));
				}
				
				//n++;
				n=1;
				prevDelta=currDelta;
			}
		}
		
	}
	
	*tot=0;
	//if(mouths->total) *tot=((n > 0) ? 1:0);
	if((mouths) && (mouths->total) && (tot)) *tot=((n < MAX_RECT) ? n:MAX_RECT);
	
	return;
}

void detect_eyes(CvMat *small_img_roi, CvRect* found_face[], CvRect* found_eyes[], 
				  CvRect* unscaled_found_face[], CvRect* unscaled_found_eyes[], int *tot, int fN, CvSize sz, int * eyes_det)
{
	CvSeq* eyes;
	int faceX, faceY, faceWidth, faceHeight;
	CvRect *r = unscaled_found_face[fN];
	int faceMidy, faceMidx;
	//double centerL=0.0, centerR=0.0;
	
	faceX = r->x;
	faceY = r->y;
	faceWidth = r->width;
	faceHeight = r->height;
	
	faceMidy = faceY + (faceHeight/2);
	faceMidx = faceX + (faceWidth/2);
	
	int currX, currY, currWidth, currHeight;
	int currMidy, currMidx;
	
	if( !leye_cascade ) return;
	
	eyes = cvHaarDetectObjects( small_img_roi, leye_cascade, storage,
							   1.1, 2, 0
							   //|CV_HAAR_FIND_BIGGEST_OBJECT
							   //|CV_HAAR_DO_ROUGH_SEARCH
							   //|CV_HAAR_DO_CANNY_PRUNING
							   //|CV_HAAR_SCALE_IMAGE
							   , cvSize(0, 0) );
	
	int nR=0;
	int nL=0;
	for( int j = 0; j < (eyes ? eyes->total : 0); j++ )
	{
		
		CvRect* er = (CvRect*)cvGetSeqElem( eyes, j );
		
		if(g_using_scaled_image) {
			er->x = (int)(((double)(er->x)) / g_using_scaled_image);
			er->y = (int)(((double)(er->y)) / g_using_scaled_image);
			er->width = (int)(((double)(er->width)) / g_using_scaled_image);
			er->height = (int)(((double)(er->height)) / g_using_scaled_image);
			
			if(g_using_scaled_image == 0.75f) {
				double myNum = (55.0f * g_using_scaled_image);
				
				er->x += (int)(myNum);
				er->y += (int)(myNum);
				er->width += (int)(myNum);
				er->height += (int)(myNum);
			}
		}		
		currX = (r->x + er->x);
		currY = (r->y + er->y);
		currWidth = er->width;
		currHeight = er->height;
		
		currMidy = currY + (currHeight/2);
		currMidx = currX + (currWidth/2);
		if((currX > (faceX + faceWidth)) || (currY > (faceY + faceHeight))) continue;
		if((currX < faceX) || (currWidth > faceWidth) || (currHeight > faceHeight)) continue;
		double myscale=scale;
#if DEBUG_FALSE_POSITIVES
//#if 1
		printf("e: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
		printf("e: %d %d %d %d\n", er->x, er->y, er->width, er->height);
		
		printf("e: (%d %d) (%d %d) (%d %d) (%d %d)\n", 
			   currMidy, faceMidy, currMidy, (int)(faceMidy/2),
			   currMidx, faceX,
			   currMidx, (faceX + (int)((float)faceWidth * (float)0.5))
			   );
		
#endif
		if((currMidy <= faceMidy) && (currMidy >= (faceMidy/2)) &&
		   (currMidx <= faceMidx) &&
		   (currWidth <= faceWidth/2) && 
		   (currMidx > faceX) && 
		   (currMidx < (faceX + (int)((float)faceWidth * (float)0.5)))) {
			
			int prevArea = (found_eyes[0]->width * found_eyes[0]->height);
			int currArea = (int)(((double)er->width* myscale) * ((double)er->height*1.25));
			
			if(currArea > prevArea) {
				found_eyes[0]->x = (int)((double)(r->x + er->x) * myscale);
				found_eyes[0]->y = (int)((double)(r->y + er->y) * myscale);
				found_eyes[0]->width = (int)((double)er->width* myscale);
				found_eyes[0]->height = (int)((double)er->height*1.25); 
				
				unscaled_found_eyes[0]->x = er->x;
				unscaled_found_eyes[0]->y = er->y;
				unscaled_found_eyes[0]->width = er->width;
				unscaled_found_eyes[0]->height = er->height; 
				
				nL = 1;
				
			}
		}
	}
	eyes = cvHaarDetectObjects( small_img_roi, reye_cascade, storage,
							   1.1, 2, 0
							   //|CV_HAAR_FIND_BIGGEST_OBJECT
							   //|CV_HAAR_DO_ROUGH_SEARCH
							   //|CV_HAAR_DO_CANNY_PRUNING
							   //|CV_HAAR_SCALE_IMAGE
							   , cvSize(0, 0) );
	for( int j = 0; j < (eyes ? eyes->total : 0); j++ )
	{
		
		CvRect* er = (CvRect*)cvGetSeqElem( eyes, j );
		
		if(g_using_scaled_image) {
			er->x = (int)(((double)(er->x)) / g_using_scaled_image);
			er->y = (int)(((double)(er->y)) / g_using_scaled_image);
			er->width = (int)(((double)(er->width)) / g_using_scaled_image);
			er->height = (int)(((double)(er->height)) / g_using_scaled_image);
			
			if(g_using_scaled_image == 0.75f) {
				double myNum = (55.0f * g_using_scaled_image);
				
				er->x += (int)(myNum);
				er->y += (int)(myNum);
				er->width += (int)(myNum);
				er->height += (int)(myNum);
			}
		}	
		
		currX = (r->x + er->x);
		currY = (r->y + er->y);
		currWidth = er->width;
		currHeight = er->height;
		
		currMidy = currY + (currHeight/2);
		currMidx = currX + (currWidth/2);
		if((currX > (faceX + faceWidth)) || (currY > (faceY + faceHeight))) continue;
		if((currX < faceX) || (currWidth > faceWidth) || (currHeight > faceHeight)) continue;
		double myscale=scale;
#if DEBUG_FALSE_POSITIVES
//#if 1
		printf("e: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
		printf("e: %d %d %d %d\n", er->x, er->y, er->width, er->height);
		
		printf("e: (%d %d) (%d %d) (%d %d) (%d %d)\n", 
			   currMidy, faceMidy, currMidy, (int)(faceMidy/2),
			   currMidx, faceX,
			   currMidx, (faceX + (int)((float)faceWidth * (float)0.5))
			   );
		
#endif
		if((currMidy <= faceMidy) && (currMidy >= (faceMidy/2)) &&
		   (currMidx >= faceMidx) &&
		   (currWidth <= faceWidth/2) && 
		   (currMidx < (faceX + faceWidth)) && 
		   (currMidx > (faceX + (int)((float)faceWidth * (float)0.5)))) {
			
			int prevArea = (found_eyes[1]->width * found_eyes[1]->height);
			int currArea = (int)(((double)er->width* myscale) * ((double)er->height*1.25));
			
			
			if(currArea > prevArea) {
				found_eyes[1]->x = (int)((double)(r->x + er->x) * myscale);
				found_eyes[1]->y = (int)((double)(r->y + er->y) * myscale);
				found_eyes[1]->width = (int)((double)er->width * myscale);
				found_eyes[1]->height = (int)((double)er->height * 1.25); 
				
				unscaled_found_eyes[1]->x = er->x;
				unscaled_found_eyes[1]->y = er->y;
				unscaled_found_eyes[1]->width = er->width;
				unscaled_found_eyes[1]->height = er->height; 
				nR = 1;
			}
		}		
	}
	if ((nL ==0) && (nR==1)) {
		
		double diff = (0.4 * faceWidth);
		double myscale=scale;
		found_eyes[0]->x = (int)(((double)(r->x + unscaled_found_eyes[1]->x) + diff) * myscale);
		found_eyes[0]->y = found_eyes[1]->y;
		found_eyes[0]->width = found_eyes[1]->width;
		found_eyes[0]->height = found_eyes[1]->height;
		
		unscaled_found_eyes[0]->x = (int)((double)(unscaled_found_eyes[1]->x) + diff);
		unscaled_found_eyes[0]->y = unscaled_found_eyes[1]->x;
		unscaled_found_eyes[0]->width = unscaled_found_eyes[1]->x;
		unscaled_found_eyes[0]->height = unscaled_found_eyes[1]->x; 
		nL = 1;
		
	}
	else if((nL==1) && (nR==0)) {
		double diff = (0.4 * faceWidth);
		double myscale=scale;
		found_eyes[1]->x = (int)(((double)(r->x + unscaled_found_eyes[0]->x) + diff) * myscale);
		found_eyes[1]->y = found_eyes[0]->y;
		found_eyes[1]->width = found_eyes[0]->width;
		found_eyes[1]->height = found_eyes[0]->height;
		
		unscaled_found_eyes[1]->x = (int)((double)(unscaled_found_eyes[0]->x) + diff);
		unscaled_found_eyes[1]->y = unscaled_found_eyes[0]->x;
		unscaled_found_eyes[1]->width = unscaled_found_eyes[0]->x;
		unscaled_found_eyes[1]->height = unscaled_found_eyes[0]->x; 
		nR = 1;
		
	}
	*tot=0;
	if((eyes) && (eyes->total) && (tot)) 
	{
		*tot=(nL + nR);
		*eyes_det = 1;
	}
	if(*tot == 2) {
		//calculate the midpt of the eyes
		g_eye_mid_pt_x = (double)(((double)(found_eyes[0]->x) + (double)(found_eyes[1]->x))/2);
		g_eye_mid_pt_y = (double)(((double)(found_eyes[0]->y) + (double)(found_eyes[1]->y))/2);
		g_eye_mid_pt_y += ((double)(found_eyes[0]->height)/2.0f);
		//g_eye_mid_pt_y = (double)(found_eyes[0]->y);
	}
#if 0				
	//printf("e: %d %d %d %d %d %d\n", currX, currY, currWidth, currHeight, currMidx, currMidy);
	printf("e: %d %d\n", (int)g_eye_mid_pt_x, (int)g_eye_mid_pt_y);
#endif
	
	
	return;
}

void detect_multipleORG( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], 
					 CvRect *found_eyes[], int *tot, 
					 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], 
					 CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
					 CvSize sz)
{
	IplImage *gray=NULL, *small_img=NULL;
    int i;

   // gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    small_img = cvCreateImage( cvSize( cvRound (img->width/scale),cvRound (img->height/scale)), 
							  img->depth, img->nChannels );

									//  cvRound (img->height/scale)), 8, 1 );
	
   // cvCvtColor( img, gray, CV_BGR2GRAY );
   // cvResize( gray, small_img, CV_INTER_LINEAR );
	cvResize( img, small_img, 0);
    //cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );
	
    if( cascade )
    {
       // double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
										   1.1, 2, 0
										   //|CV_HAAR_FIND_BIGGEST_OBJECT
										   //|CV_HAAR_DO_ROUGH_SEARCH
										   |CV_HAAR_DO_CANNY_PRUNING
										   //|CV_HAAR_SCALE_IMAGE
										   ,
										   cvSize(30, 30) );
       // t = (double)cvGetTickCount() - t;
       // printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
		tot[0]=((faces->total <MAX_RECT) ? faces->total:MAX_RECT);
        for( i = 0; (i < (faces ? faces->total : 0)) && ( i < MAX_RECT); i++ )
        {
			//CvMat *small_img_mouth;
			CvMat *small_img_nose, *small_img_eyes;
			CvMat small_img_roi;
					
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			cvGetSubRect( small_img, &small_img_roi, *r );
			
			small_img_nose = cvCloneMat(&small_img_roi);
			//small_img_mouth = cvCloneMat(&small_img_roi);
			small_img_eyes = cvCloneMat(&small_img_roi);
			
			if(g_using_scaled_image) {
				r->x = (int)(((double)(r->x)) / g_using_scaled_image);
				r->y = (int)(((double)(r->y)) / g_using_scaled_image);
				r->width = (int)(((double)(r->width)) / g_using_scaled_image);
				r->height = (int)(((double)(r->height)) / g_using_scaled_image);
			}
						
			found_face[i]->x = (int)((double)r->x * scale);
			found_face[i]->y = (int)((double)r->y * scale);
			found_face[i]->width = (int)((double)r->width * scale);
			found_face[i]->height = (int)((double)r->height * scale);
			
			unscaled_found_face[i]->x = r->x ;
			unscaled_found_face[i]->y = r->y ;
			unscaled_found_face[i]->width = r->width ;
			unscaled_found_face[i]->height = r->height;
			
			
#if DEBUG_FALSE_POSITIVES
//#if 1
			int faceX, faceY, faceWidth, faceHeight;
			int faceMidy;  
			//int faceMidx;
			faceX = r->x;
			faceY = r->y;
			faceWidth = r->width;
			faceHeight = r->height;
			
			faceMidy = faceY + (faceHeight/2);
			//faceMidx = faceX + (faceWidth/2);
			
			printf("f: %d %d %d %d %d\n", faceX, faceY, faceWidth, faceHeight, faceMidy);
#endif
						
			
			//for(int j=0; (tot[3] < 2) && (j < max_attempts) && (!bFoundEyes) && (leye_cascade); j++) {
			int totEyes=0;
			int eyes_det=0;
			detect_eyes(small_img_eyes, found_face, found_eyes, unscaled_found_face, unscaled_found_eyes, 
						&totEyes, i, sz, &eyes_det);
			if(totEyes == 2) { tot[3]=2;}
			//}
			
			//for(int j=0; (!tot[1]) && (j < max_attempts) && (!bFoundNose) && (nose_cascade); j++) {
			if(totEyes) {
				int totNoses=0;
				int my_nose_det=0;
				detect_nose(small_img_nose, found_face, found_nose, unscaled_found_face, unscaled_found_nose, 
					&totNoses, i, sz, &my_nose_det);
				if(totNoses >= 1) { 
					//bFoundNose =mytrue; 
					tot[1] = totNoses;
					//tot[1]=1;
				}
			}
			
#if 0			
			//for(int j=0; (!tot[2]) && (j < max_attempts) && (!bFoundMouth) && (mouth_cascade); j++) {
				int totMouths=0;

			if(totNoses) {
				detect_mouth(small_img_mouth, found_face, found_mouth, unscaled_found_face, unscaled_found_mouth, 
						unscaled_found_nose, &totMouths, totNoses, i, sz);
			}
			else 
			{
				detect_mouth(small_img_mouth, found_face, found_mouth, unscaled_found_face, unscaled_found_mouth, 
							 NULL, &totMouths, totNoses, i, sz);
			}

				if(totMouths >= 1) { 
					//bFoundMouth =mytrue; 
					//tot[2]=1;
					tot[2] = totMouths;
				}
			//}
			
#endif			
			
			
			//cvReleaseMat(&small_img_roi);
			cvReleaseMat(&small_img_nose);
			//cvReleaseMat(&small_img_mouth);
			cvReleaseMat(&small_img_eyes);
		}
    }
	//printf("%d %d %d %d\n", tot[0], tot[1], tot[2], tot[3]);
   // cvShowImage( "result", img );
    if(gray) cvReleaseImage( &gray );
    if(small_img) cvReleaseImage( &small_img );
}

void detect_multiple( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], 
					 CvRect *found_eyes[], int *tot, 
					 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], 
					 CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
					 CvSize sz, int *eyes_det)
{
	IplImage *gray=NULL, *small_img=NULL;
    int i;
	int face_det=0;
#if 0	
	
	gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	small_img = cvCreateImage( cvSize( cvRound (img->width/2),
										  cvRound (img->height/2)), 8, 1 );
	cvCvtColor( img, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );
#else
	small_img = img;
#endif
	cvClearMemStorage( storage );
	cascade = face_cascade;
    if( cascade )
	//if(0)
    {
		
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
										   1.1, 2, 0
										   //|CV_HAAR_FIND_BIGGEST_OBJECT
										   //|CV_HAAR_DO_ROUGH_SEARCH
										   |CV_HAAR_DO_CANNY_PRUNING
										   //|CV_HAAR_SCALE_IMAGE
										   ,
										   cvSize(30, 30) );
		
		tot[0]=((faces->total <MAX_RECT) ? faces->total:MAX_RECT);
        for( i = 0; (i < (faces ? faces->total : 0)) && ( i < MAX_RECT); i++ )
        {
						
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			CvMat *small_img_nose, *small_img_eyes;
			face_det=1;
			if(!use_quickest_search) {
				//CvMat *small_img_mouth;
				
				CvMat small_img_roi;
				
				cvGetSubRect( small_img, &small_img_roi, *r );
			
				small_img_nose = cvCloneMat(&small_img_roi);
				//small_img_mouth = cvCloneMat(&small_img_roi);
				small_img_eyes = cvCloneMat(&small_img_roi);
			}
			//if(0) {
			if(g_using_scaled_image) {
				r->x = (int)(((double)(r->x)) / g_using_scaled_image);
				r->y = (int)(((double)(r->y)) / g_using_scaled_image);
				r->width = (int)(((double)(r->width)) / g_using_scaled_image);
				r->height = (int)(((double)(r->height)) / g_using_scaled_image);
				
				if((use_quickest_search) && (g_using_scaled_image == 0.75f)) {
					double myNum = (55.0f * g_using_scaled_image);
					
					r->x += (int)(myNum);
					r->y += (int)(myNum);
					r->width += (int)(myNum);
					r->height += (int)(myNum);
				}

			}
			
			found_face[i]->x = (int)((double)r->x * scale);
			found_face[i]->y = (int)((double)r->y * scale);
			found_face[i]->width = (int)((double)r->width * scale);
			found_face[i]->height = (int)((double)r->height * scale);
			
			unscaled_found_face[i]->x = r->x ;
			unscaled_found_face[i]->y = r->y ;
			unscaled_found_face[i]->width = r->width ;
			unscaled_found_face[i]->height = r->height;
			
			
#if DEBUG_FALSE_POSITIVES
//#if 1
			int faceX, faceY, faceWidth, faceHeight;
			int faceMidy;
			//int faceMidx;
			faceX = r->x;
			faceY = r->y;
			faceWidth = r->width;
			faceHeight = r->height;
			
			faceMidy = faceY + (faceHeight/2);
			//faceMidx = faceX + (faceWidth/2);
			
			printf("f: %d %d %d %d %d\n", faceX, faceY, faceWidth, faceHeight, faceMidy);
#endif
			
			//if(!use_quickest_search) {
			if(1) {
			
				int totEyes=0;
				detect_eyes(small_img_eyes, found_face, found_eyes, unscaled_found_face, unscaled_found_eyes, 
						&totEyes, i, sz, eyes_det);
				if(totEyes == 2) { tot[3]=2; tot[1]=2;}
			
#ifdef DEBUG			
			
				if(totEyes) {
					int totNoses=0;
					detect_nose(small_img_nose, found_face, found_nose, unscaled_found_face, unscaled_found_nose, 
							&totNoses, i, sz, nose_det);
					if(totNoses >= 1) { 
					
						tot[1] = totNoses;
					
					}
				}
			
				int totMouths=0;
			
				if(totNoses) {
					detect_mouth(small_img_mouth, found_face, found_mouth, unscaled_found_face, unscaled_found_mouth, 
							 unscaled_found_nose, &totMouths, totNoses, i, sz);
				}
				else 
				{
					detect_mouth(small_img_mouth, found_face, found_mouth, unscaled_found_face, unscaled_found_mouth, 
							 NULL, &totMouths, totNoses, i, sz);
				}
			
				if(totMouths >= 1) { 
				
					tot[2] = totMouths;
				}
			
			
#endif			
			}
			
			if(!use_quickest_search) {
				//cvReleaseMat(&small_img_roi);
				cvReleaseMat(&small_img_nose);
				//cvReleaseMat(&small_img_mouth);
				cvReleaseMat(&small_img_eyes);
			}
		}
	}
	
#if 0
	if((!face_det) && (profile_face_cascade)) 
	{
		cascade = profile_face_cascade;
		CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
										   1.1, 
										   //2, 
										   2, 0
										   //|CV_HAAR_FIND_BIGGEST_OBJECT
										   //|CV_HAAR_DO_ROUGH_SEARCH
										   |CV_HAAR_DO_CANNY_PRUNING
										   //|CV_HAAR_SCALE_IMAGE
										   ,
										   cvSize(20, 20) );
		// t = (double)cvGetTickCount() - t;
		// printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
		tot[0]=((faces->total <MAX_RECT) ? faces->total:MAX_RECT);
		for( i = 0; (i < (faces ? faces->total : 0)) && ( i < MAX_RECT); i++ )
		{
			

			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			if(g_using_scaled_image) {
				r->x = (int)(((double)(r->x)) / g_using_scaled_image);
				r->y = (int)(((double)(r->y)) / g_using_scaled_image);
				r->width = (int)(((double)(r->width)) / g_using_scaled_image);
				r->height = (int)(((double)(r->height)) / g_using_scaled_image);
#ifdef DEBUG_PROFILE_FACE				
				if(g_using_scaled_image == 0.75f) {
					double myNum = (55.0f * g_using_scaled_image);
					
					r->x += (int)(myNum);
					r->y += (int)(myNum);
					r->width += (int)(myNum);
					r->height += (int)(myNum);
				}
#endif
			}
			double myscaleW=1.5f, myscaleH = 1.5f;
			
			double myW = (double)(r->width);
			double myH = (double)(r->height);
			
			found_face[i]->x = (int)((double)r->x * 1.0f);
			found_face[i]->y = (int)((double)r->y * 1.0f);
			if((unsigned int)(myW * 1.5f) < IPHONEW) {
				myscaleW = 1.5f;
			}
			else if((unsigned int)(myW * 1.25f) < IPHONEW) {
				myscaleW = 1.25f;
			}
			else if((unsigned int)(myW * 1.15f) < IPHONEW) {
				myscaleW = 1.15f;
			}
			else if((unsigned int)(myW * 1.10f) < IPHONEW) {
				myscaleW = 1.10f;
			}
			else if((unsigned int)(myW * 1.05f) < IPHONEW) {
				myscaleW = 1.05f;
			}
			
			if((unsigned int)(myH * 1.5f) < (IPHONEH - 53)) {
				myscaleH = 1.5f;
			}
			else if((unsigned int)(myH * 1.25f) < (IPHONEH-53)) {
				myscaleH = 1.25f;
			}
			else if((unsigned int)(myH * 1.15f) < (IPHONEH - 53)) {
				myscaleH = 1.15f;
			}
			else if((unsigned int)(myH * 1.10f) < (IPHONEH - 53)) {
				myscaleH = 1.10f;
			}
			else if((unsigned int)(myH * 1.05f) < (IPHONEH - 53)) {
				myscaleH = 1.05f;
			}
			
			found_face[i]->width = (int)((double)r->width * myscaleW);
			found_face[i]->height = (int)((double)r->height * myscaleH);
			
			unscaled_found_face[i]->x = r->x ;
			unscaled_found_face[i]->y = r->y ;
			unscaled_found_face[i]->width = r->width ;
			unscaled_found_face[i]->height = r->height;
			
			
//#if DEBUG_FALSE_POSITIVES
#if 1
			int faceX, faceY, faceWidth, faceHeight;
			int faceMidy;
			//int faceMidx;
			faceX = r->x;
			faceY = r->y;
			faceWidth = r->width;
			faceHeight = r->height;
			
			faceMidy = faceY + (faceHeight/2);
			//faceMidx = faceX + (faceWidth/2);
			
			printf("pf: %d %d %d %d %d\n", faceX, faceY, faceWidth, faceHeight, faceMidy);
			printf("pf%d: %d %d %d %d\n", i,found_face[i]->x, found_face[i]->y, found_face[i]->width, found_face[i]->height);
#endif
			
		}				
	}
#endif
	
    if(gray) cvReleaseImage( &gray );
#if 0
    if(small_img) cvReleaseImage( &small_img );
#endif
}

void detect_profileface_multiple( IplImage *img, 
					 CvRect* found_face[], CvRect* found_nose[], CvRect* found_mouth[], 
					 CvRect *found_eyes[], int *tot, 
					 CvRect* unscaled_found_face[], CvRect* unscaled_found_nose[], 
					 CvRect* unscaled_found_mouth[], CvRect *unscaled_found_eyes[],
					 CvSize sz, int *nose_det)
{
	IplImage *gray=NULL, *small_img=NULL;
    int i;
#if 1	
	if(!gray) {
		gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
		small_img = cvCreateImage( cvSize( cvRound (img->width/2),
										  cvRound (img->height/2)), 8, 1 );
	}	
	cvCvtColor( img, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );
#else
	small_img = img;
#endif
	
    cvClearMemStorage( storage );
	if( !profile_face_cascade ) return;
	cascade = profile_face_cascade;
    if( profile_face_cascade )
    {
		// double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
										   1.1, 
										   //2, 
										   2, 0
										   //|CV_HAAR_FIND_BIGGEST_OBJECT
										   //|CV_HAAR_DO_ROUGH_SEARCH
										   |CV_HAAR_DO_CANNY_PRUNING
										   //|CV_HAAR_SCALE_IMAGE
										   ,
										   cvSize(20, 20) );
		// t = (double)cvGetTickCount() - t;
		// printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
		tot[0]=((faces->total <MAX_RECT) ? faces->total:MAX_RECT);
        for( i = 0; (i < (faces ? faces->total : 0)) && ( i < MAX_RECT); i++ )
        {
			
#if 0
			//CvMat *small_img_mouth;
			CvMat *small_img_nose, *small_img_eyes;
			CvMat small_img_roi;
			
            
			cvGetSubRect( small_img, &small_img_roi, *r );
			
			small_img_nose = cvCloneMat(&small_img_roi);
			//small_img_mouth = cvCloneMat(&small_img_roi);
			small_img_eyes = cvCloneMat(&small_img_roi);
#endif		
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			if(g_using_scaled_image) {
				r->x = (int)(((double)(r->x)) / g_using_scaled_image);
				r->y = (int)(((double)(r->y)) / g_using_scaled_image);
				r->width = (int)(((double)(r->width)) / g_using_scaled_image);
				r->height = (int)(((double)(r->height)) / g_using_scaled_image);
#ifdef DEBUG				
				if(g_using_scaled_image == 0.75f) {
					double myNum = (55.0f * g_using_scaled_image);
					
					r->x += (int)(myNum);
					r->y += (int)(myNum);
					r->width += (int)(myNum);
					r->height += (int)(myNum);
				}
#endif
			}
			double myscaleW=1.5f, myscaleH = 1.5f;
			double myW = (double)(r->width);
			double myH = (double)(r->height);
			
			found_face[i]->x = (int)((double)r->x * 1.0f);
			found_face[i]->y = (int)((double)r->y * 1.0f);
			if((unsigned int)(myW * 1.5f) < IPHONEW) {
				myscaleW = 1.5f;
			}
			else if((unsigned int)(myW * 1.25f) < IPHONEW) {
				myscaleW = 1.25f;
			}
			else if((unsigned int)(myW * 1.15f) < IPHONEW) {
				myscaleW = 1.15f;
			}
			else if((unsigned int)(myW * 1.10f) < IPHONEW) {
				myscaleW = 1.10f;
			}
			else if((unsigned int)(myW * 1.05f) < IPHONEW) {
				myscaleW = 1.05f;
			}
			
			if((unsigned int)(myH * 1.5f) < (IPHONEH - 53)) {
				myscaleH = 1.5f;
			}
			else if((unsigned int)(myH * 1.25f) < (IPHONEH-53)) {
				myscaleH = 1.25f;
			}
			else if((unsigned int)(myH * 1.15f) < (IPHONEH - 53)) {
				myscaleH = 1.15f;
			}
			else if((unsigned int)(myH * 1.10f) < (IPHONEH - 53)) {
				myscaleH = 1.10f;
			}
			else if((unsigned int)(myH * 1.05f) < (IPHONEH - 53)) {
				myscaleH = 1.05f;
			}
			
			found_face[i]->width = (int)((double)r->width * myscaleW);
			found_face[i]->height = (int)((double)r->height * myscaleH);
			
			unscaled_found_face[i]->x = r->x ;
			unscaled_found_face[i]->y = r->y ;
			unscaled_found_face[i]->width = r->width ;
			unscaled_found_face[i]->height = r->height;
			
			
//#if DEBUG_FALSE_POSITIVES
#if 1
			int faceX, faceY, faceWidth, faceHeight;
			int faceMidy;
			//int faceMidx;
			faceX = r->x;
			faceY = r->y;
			faceWidth = r->width;
			faceHeight = r->height;
			
			faceMidy = faceY + (faceHeight/2);
			//faceMidx = faceX + (faceWidth/2);
			
			printf("pf: %d %d %d %d %d\n", faceX, faceY, faceWidth, faceHeight, faceMidy);
			printf("pf%d: %d %d %d %d\n", i,found_face[i]->x, found_face[i]->y, found_face[i]->width, found_face[i]->height);
#endif
			
		}
    }
	//printf("%d %d %d %d\n", tot[0], tot[1], tot[2], tot[3]);
	// cvShowImage( "result", img );
    if(gray) cvReleaseImage( &gray );
#if 1
    if(small_img) cvReleaseImage( &small_img );
#endif
}


