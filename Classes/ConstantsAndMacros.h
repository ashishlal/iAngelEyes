/*
 *  ConstantsAndMacros.h
 *  iAngelEyes
 *
 *  Created by Ashish Lal on 01/06/10.
 *  Copyright 2010 NetTech India. All rights reserved.
 *
 */

#ifndef _CONSTS_MACROS_H_
#define _CONSTS_MACROS_H_

// How many times a second to refresh the screen
#define kRenderingFrequency 60.0

// For setting up perspective, define near, far, and angle of view
#define kZNear			0.01
#define kZFar			1000.0
#define kFieldOfView	45.0

#define kGroupIndexVertex 0

// Macros
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define kGroupIndexVertex			0
#define kGroupIndexTextureCoordIndex	1
#define kGroupIndexNormalIndex			2

// Macros
#define CONVERT_UV_U_TO_ST_S(u) ((2*u) / M_PI)

#define INIT_FACE_CASCADE 1 
#define INIT_PROFILE_FACE_CASCADE (1 << 1)
#define INIT_LEYE_CASCADE (1 << 2)
#define INIT_REYE_CASCADE (1 << 3)
#define INIT_NOSE_CASCADE (1 << 4)
#define INIT_MOUTH_CASCADE (1 << 4)

enum {
	FIRST_OBJECT=0,
	PREV_OBJECT,
	NEXT_OBJECT,
	LAST_OBJECT
};

enum {
	NONE=0,
	AMBIENT=1,
	DIFFUSE,
	SPECULAR,
	SHININESS,
	TEXTURE,
	ALPHA
};


enum {
	EURO_FEM_NOSE_BRIDGE_LONG=0,
	EURO_FEM_NOSE_BRIDGE_DEEP,
	EURO_FEM_NOSE_BRIDGE_SHORT,
	EURO_FEM_NOSE_BRIDGE_SHALLOW,
	EURO_FEM_NOSE_DOWN,
	EURO_FEM_NOSE_FLAT,	
	EURO_FEM_NOSE_LONG,
	EURO_FEM_NOSE_POINTED,
	EURO_FEM_NOSE_REGION_CONCAVE,
	EURO_FEM_NOSE_REGION_CONVEX,
	EURO_FEM_NOSE_SELLION_DEEP,
	EURO_FEM_NOSE_SELLION_DOWN,
	EURO_FEM_NOSE_SELLION_SHALLOW,
	EURO_FEM_NOSE_SELLION_THIN,
	EURO_FEM_NOSE_SELLION_UP,
	EURO_FEM_NOSE_SELLION_WIDE,
	EURO_FEM_NOSE_SHORT,
	EURO_FEM_NOSE_TILT_DOWN,
	EURO_FEM_NOSE_TILT_UP,
	MaxNoseSurfaceCount,
	EURO_FEM_NOSE_UP,
	EURO_FEM_NOSTRIL_TILT_DOWN,
	
	EURO_FEM_NOSE_NOSTRIL_TILT_UP,
	EURO_FEM_NOSTRILS_LARGE,
	EURO_FEM_NOSTRILS_SMALL,
	EURO_FEM_NOSTRILS_THIN,
	EURO_FEM_NOSTRILS_WIDE,
	EURO_NOSE_AVG,
	EURO_NOSE_BRIDGE_DEEP,
	EURO_NOSE_BRIDGE_LONG,
	EURO_NOSE_BRIDGE_SHALLOW,
	EURO_NOSE_BRIDGE_SHORT,
	EURO_NOSE_DOWN,
	EURO_NOSE_FEM_AVG,
	EURO_NOSE_FLAT,
	EURO_NOSE_LONG,
	//EURO_NOSE_MALE_AVG,
	//EURO_NOSE_NOSTRIL_DOWN,
	EURO_NOSE_NOSTRIL_THIN,
	EURO_NOSE_NOSTRIL_UP,
	EURO_NOSE_NOSTRIL_WIDE,
	EURO_NOSE_POINTED,
	EURO_NOSE_REGION_CONCAVE,
	EURO_NOSE_REGION_CONVEX,
	EURO_NOSE_SELLION_DEEP,
	EURO_NOSE_SELLION_DOWN,
	EURO_NOSE_SELLION_SHALLOW,
	EURO_NOSE_SELLION_THIN,
	EURO_NOSE_SELLION_UP,
	EURO_NOSE_SELLION_WIDE,
	EURO_NOSE_SHORT,
	EURO_NOSE_TILT_DOWN,
	EURO_NOSE_TILT_UP,
	EURO_NOSE_UP
	//MaxNoseSurfaceCount
};

enum {
	EYE_128_BLUE7,
	EYE_110,
	EYE_111,
	EYE_115,
	EYE_112,
	EYE_116,
	EYE_113,
	EYE_117,
	EYE_118,
	EYE_119,
	EYE_114,
	EYE_120,
	EYE_121,
	EYE_122,
	EYE_123,
	EYE_124,
	EYE_125,
	EYE_126,
	EYE_127,
	EYE_128,
	EYE_129,
	EYE_130,
	EYE_131,
	EYE_132,
	EYE_133,
	EYE_134,
	EYE_135,
	EYE_136,
	EYE_137,
	EYE_138,
	EYE_139,
	EYE_140,
	EYE_141,
	EYE_GEN_100,
	EYE_128_BLUE8,
	EYE_128_PURPLE1,
	EYE_128_PURPLE2,
	EYE_128_DEEPBLUE,
	EYE_128_BROWN1,
	EYE_128_BROWN2,
	EYE_128_BROWN3,
	EYE_128_RED1,
	EYE_128_RED2,
	MaxEyeSurfaceCount,
	EYE_256_BLUE7,
	EYE_256_BLUE8,
	EYE_256_PURPLE1,
	EYE_256_PURPLE2,
	EYE_256_DEEPBLUE,
	EYE_256_BROWN1,
	EYE_256_BROWN2,
	EYE_256_BROWN3,
	EYE_256_RED1,
	MY_MARK1,
	EYE_256_RED2,
	EYE_41,
	EYE_42,
	EYE_43,
	EYE_44,
	EYE_45,
	EYE_46,
	EYE_47,
	EYE_48,
	EYE_49,
	EYE_50,
	EYE_51,
	EYE_256_BLUE1,
	EYE_256_BLUE2,
	EYE_256_BLUE3,
	EYE_256_BLUE4,
	EYE_256_BLUE5,
	EYE_256_BLUE6,
	
	EYE_256_PURPLE3,
	
	
	EYE_256_GREEN1,
	EYE_256_GREEN2,
	EYE_256_GREEN3,
	EYE_256_GREEN4,
	EYE_256_GREEN5,
	EYE_256_GREEN6,
	EYE_256_GREEN7,
	EYE_256_GREEN8,
	EYE_256_GREEN9,
	
	
	
};

enum AppStates_e {
	IDLE,
	INITIALZING,
	READY,
	PROCESSING,
	FACE_DETECTED,
	NOSE_DETECTED,
	STOPPED
};

enum ViewTag_e {
	CAMERA_BAR_TAG=61,
	PREVIEW_BAR_TAG,
	PREVIEWMORE_BAR_TAG,
	BROWSE_BAR_TAG,
	NOSE_DISPLAY_TAG,
	SLIDER_TAG,
	COLOR_BAR_TAG,
	COLORMORE_BAR_TAG,
	LIGHT_BAR_TAG,
	SPINNING_TAG,
	INFO_TAG,
	NAVIGATION_TAG
};

#define MAX_FILTER_RADIUS 2
enum {
	APPLE_texture_2D_limited_npot,
	IMG_texture_format_BGRA8888,
	NUM_EXTENSIONS
};

enum Light_e {
	L0=0,
	L1,
	L2,
	L3,
	L4,
	MAX_LIGHT
};

#define MAX_RECT 5
const int SurfaceCount = 2;

#if defined __arm__ || defined __thumb__
#undef TARGET_IPHONE_SIMULATOR
#ifndef TARGET_OS_IPHONE
#define TARGET_OS_IPHONE `
#endif
#else
#define TARGET_IPHONE_SIMULATOR 1
#undef TARGET_OS_IPHONE
#endif

#endif
