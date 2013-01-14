#pragma once

#include "Vector.hpp"
#include <vector>
#include <string>
//#include "NTIMalloc.h"

using std::vector;
using std::string;

typedef struct {
	float	red;
	float	green;
	float	blue;
	float   alpha;
} Color3D;
static inline Color3D Color3DMake(float inRed, float inGreen, float inBlue, float inAlpha)
{
    Color3D ret;
	ret.red = inRed;
	ret.green = inGreen;
	ret.blue = inBlue;
	ret.alpha = inAlpha;
    return ret;
}

typedef struct {
	float x;
	float y;
	float z;
} Vertex3D;

static inline Vertex3D Vertex3DMake(float inX, float inY, float inZ)
{
	Vertex3D ret;
	ret.x = inX;
	ret.y = inY;
	ret.z = inZ;
	return ret;
}

typedef Vertex3D Vector3D;
#define Vector3DMake(x,y,z) (Vector3D)Vertex3DMake(x, y, z)
static inline float Vector3DMagnitude(Vector3D v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)); 
}
static inline void Vector3DNormalize(Vector3D *v)
{
	float vecMag = Vector3DMagnitude(*v);
	if ( vecMag == 0.0 )
	{
		v->x = 1.0;
		v->y = 0.0;
		v->z = 0.0;
	}
	

	v->x /= vecMag;
	v->y /= vecMag;
	v->z /= vecMag;
	
}

// --------------------------------------------------------------------------------------------
// This is an implementation of the famous Quake fast inverse square root algorithm. Although
// it comes from the Quake 3D code, which was released under the GPL, John Carmack has stated
// that this code was not written by him or his ID counterparts. The actual origins of this
// algorithm have never been definitively found, but enough different people have contributed
// to it that I believe it is safe to assume it's in the public domain.
// --------------------------------------------------------------------------------------------
static inline float InvSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;	// store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1);		// initial guess for Newton's method
	x = *(float*)&i;				// convert new bits into float
	x = x*(1.5f - xhalf*x*x);		// One round of Newton's method
	return x;
}
// END Fast invqrt code -----------------------------------------------------------------------
static inline float Vector3DFastInverseMagnitude(Vector3D vector)
{
	return InvSqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
}

static inline void Vector3DFastNormalize(Vector3D *vector)
{
	float vecInverseMag = Vector3DFastInverseMagnitude(*vector);
	if (vecInverseMag == 0.0)
	{
		vector->x = 1.0;
		vector->y = 0.0;
		vector->z = 0.0;
	}
	vector->x *= vecInverseMag;
	vector->y *= vecInverseMag;
	vector->z *= vecInverseMag;
}

static inline float Vector3DDotProduct(Vector3D vector1, Vector3D vector2)
{		
	return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;
	
}
static inline Vector3D Vector3DCrossProduct(Vector3D vector1, Vector3D vector2)
{
	Vector3D ret;
	ret.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
	ret.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
	ret.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
	return ret;
}
static inline Vector3D Vector3DMakeWithStartAndEndPoints(Vector3D start, Vector3D end)
{
	Vector3D ret;
	ret.x = end.x - start.x;
	ret.y = end.y - start.y;
	ret.z = end.z - start.z;
	Vector3DNormalize(&ret);
	return ret;
}
static inline Vector3D Vector3DAdd(Vector3D vector1, Vector3D vector2)
{
	Vector3D ret;
	ret.x = vector1.x + vector2.x;
	ret.y = vector1.y + vector2.y;
	ret.z = vector1.z + vector2.z;
	return ret;
}
static inline void Vector3DFlip (Vector3D *vector)
{
	vector->x = -vector->x;
	vector->y = -vector->y;
	vector->z = -vector->z;
}

// A Rotation3D is just a Vertex3D used to store three angles (pitch, yaw, roll) instead of cartesian coordinates. 
// For simplicity, we just reuse the Vertex3D, even though the member names should probably be either xRot, yRot, 
// and zRot, or else pitch, yaw, roll. 
typedef Vector3D Rotation3D;
#define Rotation3DMake(x,y,z) (Rotation3D)Vertex3DMake(x, y, z)
#pragma mark -
#pragma mark Face3D
#pragma mark -
// Face3D is used to hold three integers which will be integer index values to another array
typedef struct {
	unsigned short	v1;
	unsigned short	v2;
	unsigned short	v3;
} Face3D;
static inline Face3D Face3DMake(unsigned short v1, unsigned short v2, unsigned short v3)
{
	Face3D ret;
	ret.v1 = v1;
	ret.v2 = v2;
	ret.v3 = v3;
	return ret;
}

typedef struct {
	Vector3D v1;
	Vector3D v2;
	Vector3D v3;
} Triangle3D;
static inline Triangle3D Triangle3DMake(Vector3D inV1, Vector3D inV2, Vector3D inV3)
{
	Triangle3D ret;
	ret.v1 = inV1;
	ret.v2 = inV2;
	ret.v3 = inV3;
	return ret;
}

static inline Vector3D Triangle3DCalculateSurfaceNormal(Triangle3D triangle)
{
	Vector3D u = Vector3DMakeWithStartAndEndPoints(triangle.v2, triangle.v1);
	Vector3D v = Vector3DMakeWithStartAndEndPoints(triangle.v3, triangle.v1);
	
	Vector3D ret;

	ret.x = (u.y * v.z) - (u.z * v.y);
	ret.y = (u.z * v.x) - (u.x * v.z);
	ret.z = (u.x * v.y) - (u.y * v.x);

	return ret;
}

// This implements a binary search tree that will help us determine which vertices need to be duplicated. In
// OpenGL, each vertex has to have one and only one set of texture coordinates, so if a single vertex is shared
// by multiple triangles and has different texture coordinates in each, those vertices need to be duplicated
// so that there is one copy of that vertex for every distinct set of texture coordinates.

// This works with index values, not actual Vertex3D values, for speed, and because that's the way the 
// OBJ file format tells us about them
//
// An actualVertex value of UINT_MAX means that the actual integer value hasn't been determined yet. 
typedef struct {
	unsigned int	originalVertex;
	unsigned int	textureCoords;
	unsigned int	actualVertex;
	void	*greater;
	void	*lesser;
	
} VertexTextureIndex;
static inline VertexTextureIndex * VertexTextureIndexMake (unsigned int inVertex, unsigned int inTextureCoords, 
														   unsigned int inActualVertex)
{

#ifdef MALLOC_DEBUG
	// +++ lal: added 100
	VertexTextureIndex *ret = (VertexTextureIndex *) NTIMalloc(sizeof(VertexTextureIndex), (char *)__FILE__, __LINE__);
#else
	VertexTextureIndex *ret = (VertexTextureIndex *) malloc(sizeof(VertexTextureIndex));
#endif
	if(!ret) return NULL;
	memset(ret, 0, sizeof(VertexTextureIndex));
	ret->originalVertex = inVertex;
	ret->textureCoords = inTextureCoords;
	ret->actualVertex = inActualVertex;
	ret->greater = NULL;
	ret->lesser = NULL;
	return ret;
}
#define VertexTextureIndexMakeEmpty(x,y) VertexTextureIndexMake(x, y, UINT_MAX)
// recursive search function - looks for a match for a given combination of vertex and 
// texture coordinates. If not found, returns UINT_MAX
static inline unsigned int VertexTextureIndexMatch(VertexTextureIndex *node, 
											 unsigned int matchVertex, unsigned int matchTextureCoords)
{
	if (node->originalVertex == matchVertex && node->textureCoords == matchTextureCoords)
		return node->actualVertex;
	
	if (node->greater != NULL)
	{
		unsigned int greaterIndex = VertexTextureIndexMatch((VertexTextureIndex *)(node->greater), matchVertex, matchTextureCoords);
		if (greaterIndex != UINT_MAX)
			return greaterIndex;
	}
	
	if (node->lesser != NULL)
	{
		unsigned int lesserIndex = VertexTextureIndexMatch((VertexTextureIndex *)(node->lesser), matchVertex, matchTextureCoords);
		return lesserIndex;
	}
	return UINT_MAX;
}
static inline VertexTextureIndex * VertexTextureIndexAddNode(VertexTextureIndex *node, unsigned int newVertex, 
															 unsigned int newTextureCoords)
{
	// If requested new node matches the one being added to, then don't add, just return pointer to match
	if (node->originalVertex == newVertex && node->textureCoords == newTextureCoords)
		return node;
	if (node->originalVertex > newVertex || (node->originalVertex == newVertex && node->textureCoords > newTextureCoords))
	{
		if (node->lesser != NULL)
			return VertexTextureIndexAddNode((VertexTextureIndex *) (node->lesser), newVertex, newTextureCoords);
		else
		{
			VertexTextureIndex *newNode = VertexTextureIndexMakeEmpty(newVertex, newTextureCoords);
			node->lesser = newNode;
			return (VertexTextureIndex *)(node->lesser);
		}
	}
	else
	{
		if (node->greater != NULL)
			return VertexTextureIndexAddNode((VertexTextureIndex *)(node->greater), newVertex, newTextureCoords);
		else
		{
			VertexTextureIndex *newNode = (VertexTextureIndex *)VertexTextureIndexMakeEmpty(newVertex, newTextureCoords);
			node->greater = newNode;
			return (VertexTextureIndex *)(node->greater);
		}	
	}
	return NULL; // shouldn't ever reach here.
}
static inline bool VertexTextureIndexContainsVertexIndex(VertexTextureIndex *node, unsigned int matchVertex)
{
	if (node->originalVertex == matchVertex)
		return true;
	
	bool greaterHas = false;
	bool lesserHas = false;
	
	if (node->greater != NULL)
		greaterHas = VertexTextureIndexContainsVertexIndex((VertexTextureIndex *)(node->greater), matchVertex);
	if (node->lesser != NULL)
		lesserHas = VertexTextureIndexContainsVertexIndex((VertexTextureIndex *)(node->lesser), matchVertex);
	return lesserHas || greaterHas;
}
static inline void VertexTextureIndexFree(VertexTextureIndex *node)
{
	if (node != NULL)
	{
		 
		if (node->greater != NULL)
			VertexTextureIndexFree((VertexTextureIndex *)(node->greater));
		if (node->lesser != NULL)
			VertexTextureIndexFree((VertexTextureIndex *)(node->lesser));
		//cout << "free " << __LINE__ << " " << node << endl;
#ifdef MALLOC_DEBUG
		NTIFree(node, (char *) __FILE__, __LINE__);
#else
		free(node);
#endif
		//node=NULL;
	}
	
}
static inline unsigned int VertexTextureIndexCountNodes(VertexTextureIndex *node)
{
	unsigned int ret = 0;  
	
	if (node)
	{
		ret++; // count this node
		
		// Add the children
		if (node->greater != NULL)
			ret += VertexTextureIndexCountNodes((VertexTextureIndex *)(node->greater));
		if (node->lesser != NULL)
			ret += VertexTextureIndexCountNodes((VertexTextureIndex *)(node->lesser));
	}
	return ret;
}