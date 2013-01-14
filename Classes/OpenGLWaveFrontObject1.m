//
//  OpenGLWaveFrontObject.m
//  Wavefront OBJ Loader
//
//  Created by Jeff LaMarche on 12/14/08.
//  Copyright 2008 Jeff LaMarche Consulting. All rights reserved.
//
// This file will load certain .obj files into memory and display them in OpenGL ES.
// Because of limitations of OpenGL ES, not all .obj files can be loaded - faces larger
// than triangles cannot be handled, so files must be exported with only triangles.

// This version doesn't handle textures. That's coming.
#if 0
#import "OpenGLWaveFrontObject.h"

// How many times a second to refresh the screen
#define kRenderingFrequency 60.0

// For setting up perspective, define near, far, and angle of view
#define kZNear			0.01
#define kZFar			1000.0
#define kFieldOfView	45.0

#define kGroupIndexVertex 0

// Macros
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)

void perspective(double fovy, double aspect, double zNear, double zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	double xmin, xmax, ymin, ymax;
	
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	
	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
	
	
	
	glMatrixMode(GL_MODELVIEW);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	
	glDepthMask(GL_TRUE);
}

void orthographic()		   
{                   
	glMatrixMode(GL_PROJECTION);		   
	glLoadIdentity();					   
	glOrthof( 0, 320, 480, 0, 1, 0 );				
	glMatrixMode(GL_MODELVIEW);				
	glLoadIdentity();					
	glDepthMask(GL_FALSE);
}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
			   
			   GLfloat centerx, GLfloat centery, GLfloat centerz,
			   
			   GLfloat upx, GLfloat upy, GLfloat upz)

{
	
    GLfloat m[16];
	
    GLfloat x[3], y[3], z[3];
	
    GLfloat mag;
	
	
	
    /* Make rotation matrix */
	
	
	
    /* Z vector */
	
    z[0] = eyex - centerx;
	
    z[1] = eyey - centery;
	
    z[2] = eyez - centerz;
	
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	
    if (mag) {                   /* mpichler, 19950515 */
		
		z[0] /= mag;
		
		z[1] /= mag;
		
		z[2] /= mag;
		
    }
	
	
	
    /* Y vector */
	
    y[0] = upx;
	
    y[1] = upy;
	
    y[2] = upz;
	
	
	
    /* X vector = Y cross Z */
	
    x[0] = y[1] * z[2] - y[2] * z[1];
	
    x[1] = -y[0] * z[2] + y[2] * z[0];
	
    x[2] = y[0] * z[1] - y[1] * z[0];
	
	
	
    /* Recompute Y = Z cross X */
	
    y[0] = z[1] * x[2] - z[2] * x[1];
	
    y[1] = -z[0] * x[2] + z[2] * x[0];
	
    y[2] = z[0] * x[1] - z[1] * x[0];
	
	
	
    /* mpichler, 19950515 */
	
    /* cross product gives area of parallelogram, which is < 1.0 for
	 
     * non-perpendicular unit-length vectors; so normalize x, y here
	 
     */
	
	
	
    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	
    if (mag) {
		
		x[0] /= mag;
		
		x[1] /= mag;
		
		x[2] /= mag;
		
    }
	
    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	
    if (mag) {
		
		y[0] /= mag;
		
		y[1] /= mag;
		
		y[2] /= mag;
		
    }
	
	
	
#define M(row,col)  m[col*4+row]
	
    M(0, 0) = x[0];
	
    M(0, 1) = x[1];
	
    M(0, 2) = x[2];
	
    M(0, 3) = 0.0;
	
    M(1, 0) = y[0];
	
    M(1, 1) = y[1];
	
    M(1, 2) = y[2];
	
    M(1, 3) = 0.0;
	
    M(2, 0) = z[0];
	
    M(2, 1) = z[1];
	
    M(2, 2) = z[2];
	
    M(2, 3) = 0.0;
	
    M(3, 0) = 0.0;
	
    M(3, 1) = 0.0;
	
    M(3, 2) = 0.0;
	
    M(3, 3) = 1.0;
	
#undef M
	
    glMultMatrixf(m);
	
	
	
    /* Translate Eye to Origin */
	
    glTranslatef(-eyex, -eyey, -eyez);
	
	
	
}

@implementation OpenGLWaveFrontObject
@synthesize currentPosition;
@synthesize currentRotation;
- (id)initWithPath:(NSString *)path
{
	if ((self = [super init]))
	{

		NSString *objData = [NSString stringWithContentsOfFile:path];
		NSUInteger vertexCount = 0, faceCount = 0;
		// Iterate through file once to discover how many vertices, normals, and faces there are
		NSArray *lines = [objData componentsSeparatedByString:@"\n"];
		for (NSString * line in lines)
		{
			if ([line hasPrefix:@"v "])
				vertexCount++;
			else if ([line hasPrefix:@"f "])
				faceCount++;
		}
		NSLog(@"Vertices: %d, Normals: %d, Faces: %d", vertexCount, faceCount);
		vertices = malloc(sizeof(Vertex3D) * vertexCount);
		faces = malloc(sizeof(Face3D) * faceCount);
		
		// Reuse our count variables for second time through
		vertexCount = 0;
		faceCount = 0;
		for (NSString * line in lines)
		{
			if ([line hasPrefix:@"v "])
			{
				NSString *lineTrunc = [line substringFromIndex:2];
				NSArray *lineVertices = [lineTrunc componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
				vertices[vertexCount].x = [[lineVertices objectAtIndex:0] floatValue];
				vertices[vertexCount].y = [[lineVertices objectAtIndex:1] floatValue];
				vertices[vertexCount].z = [[lineVertices objectAtIndex:2] floatValue];
				// Ignore weight if it exists..
				vertexCount++;
			}
			else if ([line hasPrefix:@"f "])
			{
				NSString *lineTrunc = [line substringFromIndex:2];
				NSArray *faceIndexGroups = [lineTrunc componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
				// Unrolled loop, a little ugly but functional
				/*
				 From the WaveFront OBJ specification:
				 o       The first reference number is the geometric vertex.
				 o       The second reference number is the texture vertex. It follows the first slash.
				 o       The third reference number is the vertex normal. It follows the second slash.
				 */
				NSString *oneGroup = [faceIndexGroups objectAtIndex:0];
				NSArray *groupParts = [oneGroup componentsSeparatedByString:@"/"];
				faces[faceCount].v1 = [[groupParts objectAtIndex:kGroupIndexVertex] intValue]-1; // indices in file are 1-indexed, not 0 indexed
				oneGroup = [faceIndexGroups objectAtIndex:1];
				groupParts = [oneGroup componentsSeparatedByString:@"/"];
				faces[faceCount].v2 = [[groupParts objectAtIndex:kGroupIndexVertex] intValue]-1;	
				oneGroup = [faceIndexGroups objectAtIndex:2];
				groupParts = [oneGroup componentsSeparatedByString:@"/"];
				faces[faceCount].v3 = [[groupParts objectAtIndex:kGroupIndexVertex] intValue]-1;
				faceCount++;
				
			}
		}
		numberOfFaces = faceCount;
		
	}
	return self;
}

-(void)switchToOrtho 
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
	glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, 10.0f, 50.0f);
    //glOrthof(0, 320, 0, 480, 10, 50);       
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

-(void)switchBackToFrustum 
{
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

- (void)drawSelf
{
	double  zNear = 0.1;
	double zFar = 0.0;
	double diam = 160.0f;
	double left =0.0, right=0.0, bottom=0.0, top=0.0;
	double zoomFactor = 1.0f;
	
	zFar = zNear + diam;
	
	left = - diam;
	right =  diam;
	bottom =  - diam;
	top =  diam;
	
	double aspect = 320.0f / 480.0f;
	
	if ( aspect < 1.0 ) { // window taller than wide
		bottom /= aspect;
		top /= aspect;
	} else {
		left *= aspect;
		right *= aspect;
	}
	
	//+++lal: added this line
	//glMatrixMode(GL_PROJECTION);
	
	// Save the current transformation by pushing it on the stack
	glPushMatrix();
	
	// Load the identity matrix to restore to origin
	glLoadIdentity();

#if 0
	// +++lal
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, -100.0f, 1000.0f);
	glOrthof(left*zoomFactor, right*zoomFactor, bottom*zoomFactor, top*zoomFactor, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(currentPosition.x, currentPosition.y, currentPosition.z);
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, -100.0f, 1000.0f);
	//perspective(85.0f, 480/320, 0.01f, 1000.0f);
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, 10.0f, 50.0f);
	//glOrthof(-160.0f, 160.0f, -240.0f, 240.0f, 0.1f, 50.0f);
	//glPerspective(85, 1.5, 1, 5);
	//[self switchToOrtho];
	//gluLookAt (0.0f, 0.0f, 2.0f* diam/8.0f,
	gluLookAt (
			   0.0f, 0.0f, 2.0f* diam/8.0f,
			  //currentPosition.x, currentPosition.y, 2.0f* diam/8.0f,
			  0.0f, 0.0f, 0.0f,
			   0.0, 1.0, 0.0);
	//
	///gluLookAt(0,0,2,
	//gluLookAt(0,0,2,
	//		  0, 0, 0,
			 //currentPosition.x, currentPosition.y, currentPosition.z,
	//		  0,1,0);
	//NSLog(@"currP1-d:%.0f %.0f %.0f",currentPosition.x, currentPosition.y, currentPosition.z);
	//glLoadIdentity();
	// Translate to the current position
	float mysc =  diam/16.0f;
	glTranslatef(currentPosition.x/mysc, currentPosition.y/mysc, 0.0f);
	//glScalef(2.0f, 2.0f, 1.0f);
	NSLog(@"currP1-d:%.0f %.0f %.0f",currentPosition.x, currentPosition.y, currentPosition.z);
#else
	glTranslatef(currentPosition.x, currentPosition.y, currentPosition.z);
#endif
	// Rotate to the current rotation
	//glRotatef(currentRotation.x, 1.0, 0.0, 0.0);
	//glRotatef(currentRotation.y, 0.0, 1.0, 0.0);
	//glRotatef(currentPosition.z, 0.0, 0.0, 1.0);
	
	
	// Enable and load the vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices); 
	
	// Loop through faces and draw them
	for (int i = 0; i < numberOfFaces; i++)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &faces[i]); 
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	
	
	
	// Restore the current transformation by popping it off
	glPopMatrix();
	
	//[self switchBackToFrustum];
}
- (void)dealloc
{
	if (vertices)
		free(vertices);
	if (faces)
		free(faces);
	[super dealloc];
}
@end
#endif