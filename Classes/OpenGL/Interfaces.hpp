#pragma once
#include "Vector.hpp"
#include "Quaternion.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

enum VertexFlags {
    VertexFlagsNormals = 1 << 0,
    VertexFlagsTexCoords = 1 << 1
};

enum {
	MAC_FILE = 1 << 0,
	UNIX_FILE = 1 << 1,
	WINDOWS_FILE = 1 << 2
};

enum TextureFormat {
    TextureFormatGray,
    TextureFormatGrayAlpha,
    TextureFormatRgb,
    TextureFormatRgba,
    TextureFormatPvrtcRgb2,
    TextureFormatPvrtcRgba2,
    TextureFormatPvrtcRgb4,
    TextureFormatPvrtcRgba4,
    TextureFormat565,
    TextureFormat5551,
};

struct TextureDescription {
    TextureFormat Format;
    int BitsPerComponent;
    ivec2 Size;
    int MipCount;
};


typedef struct _Texture_s {
	unsigned int		texture[1];  
	char	filename[128];
	char	filetype[16];
} Texture;

typedef struct _Material_s {
	
	char name[128];
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float shininess;
	float opticalDensity;
	float dissolve;
	float illumination;
	Texture * texture;
	
} Material, Material_t;

typedef struct {
	unsigned short	v1;
	unsigned short	v2;
	unsigned short	v3;
} Face;

typedef struct _Group_s {
	char name[128];
	unsigned int numberOfFaces;
	Face* faces;
	Material *material;
	bool oneTexture;
	unsigned int texture[1];
} Group;

struct IApplicationEngine {
    virtual void Initialize(int width, int height) = 0;
    virtual void Render()  = 0;
    virtual void UpdateAnimation(float timeStep) = 0;
    virtual void OnFingerUp(vec2 location) = 0;
    virtual void OnFingerDown(vec2 location) = 0;
    virtual void OnFingerMove(vec2 oldLocation, vec2 newLocation) = 0;
	virtual void OnTwoFingerUp(vec2 loc, vec2 loc1) =0;
    virtual void OnTwoFingerDown(vec2 loc, vec2 loc1)=0;
    virtual void OnTwoFingerMove(vec2 loc, vec2 loc1)=0;		
	virtual int SetupScreenPos(int surfaceIndex, float x, float y, float z, float w) =0;
	virtual int BrowseObjects(int)=0;
	virtual void GetSurfaceScreenPos(int surfaceIndex, float *x, float *y, float *z, float *w) =0;
	virtual void MoveObject() =0;
	virtual void RotateObject() =0;
	virtual void ResizeObject() =0;
	virtual bool PresentBuffer() = 0;
	virtual void ResetMovement() = 0;
	virtual void PreserveOrientation(int val) =0;
	virtual void SetZoomFactor(float val)=0;
    virtual ~IApplicationEngine() {}
};

#ifdef USE_ORIG_OBJ
struct ISurface {
    virtual int GetVertexCount() const = 0;
    virtual int GetLineIndexCount() const = 0;
    virtual int GetTriangleIndexCount() const = 0;
    virtual void GenerateVertices(vector<float>& vertices,
                                  unsigned char flags = 0)  = 0;
    virtual void GenerateLineIndices(vector<unsigned short>& indices) const = 0;
    virtual void GenerateTriangleIndices(vector<unsigned short>& indices) const = 0;
	virtual vec4 GetBoundingSphere() =0;
	virtual void GetGroupAndVertices(int index, vector<Group *> * g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
									vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
									unsigned int * ) =0;
	virtual void GetGroupAndVerticesOLD(vector<Group *> * g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
									 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
									 unsigned int * ) =0;
	virtual void GetGroupMaterial(vector<Group *> *g)=0;
	virtual void InitVertices(int surfaceIndex) =0;
    virtual ~ISurface() {}
};
#else
struct ISurface {
   
	virtual vec4 GetBoundingSphere() =0;
#if 0
	virtual void GetGroupAndVertices(int index, Group **g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
									 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
									 unsigned int * ) =0;
	virtual void GetGroupAndVerticesOLD( Group **g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
										vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
										unsigned int * ) =0;
#else
	virtual void GetGroupAndVertices(int index, vector<Group *> * g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
									 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
									 unsigned int *, int ) =0;
	virtual void GetGroupAndVerticesOLD(vector<Group *> * g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
										vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord,
										unsigned int * ) =0;
#endif
	virtual void InitVertices(int surfaceIndex) =0;
	virtual ~ISurface() {};
};

#endif
struct Visual {
    vec3 Color;
    ivec2 LowerLeft;
    ivec2 ViewportSize;
    Quaternion Orientation;
	vec2 ScreenPos;
};

struct IRenderingEngine {
    virtual void Initialize(const vector<ISurface*>& surfaces) = 0;
	virtual void InitializeBoundingSphere(ISurface *surface, int surfaceIndex) = 0;
	virtual void InitializeMY(const vector<ISurface*>& surfaces) = 0;
    virtual void Render(const vector<Visual>& visuals)  = 0;
	//virtual void RenderMY(const vector<Visual>& visuals, int)  = 0;
	virtual void RenderMY(const Visual *visual)  = 0;
	virtual bool PresentBuffer()  = 0;
	virtual void SetNoseTextureData(void *, int, int)=0;
	virtual void SetMatProperty(int p, float value) =0;
	virtual void DeallocCurrentObject()=0;
	virtual void SetAlphaCorrection(int val)=0;
	virtual void ToggleLight(int val)=0;
	virtual void ClearBuffer() = 0;
	virtual void SetSurfaceTypeIndex(int n) =0;
    virtual ~IRenderingEngine() {}
};

struct IResourceManager {
    virtual string GetResourcePath() const = 0;
    virtual TextureDescription LoadPngImage(const string& filename) = 0;
	virtual TextureDescription LoadPvrImage(const string& filename) = 0;
	virtual void* GetImageData() = 0;
    virtual void UnloadImage() = 0;
#if 0
	virtual void LoadPpngImage(const string& name, void **cgimage, void **data, unsigned int *component, 
							   unsigned int *format, int *w, int *h)=0;
	virtual void LoadJpgImage(const string& filename) = 0;
	virtual void* GetJpgImageData(void **) = 0;
	virtual void GetCGIImage(void  **cgi)=0;
    virtual ivec2 GetImageSize() = 0;
	
	virtual void UnloadJpgImage() = 0;
#endif
    virtual ~IResourceManager() {}
};

namespace ParametricViewer { IApplicationEngine* CreateApplicationEngine(IRenderingEngine*); }
namespace ObjViewer    { IApplicationEngine* CreateApplicationEngine(IRenderingEngine*, IResourceManager*); }
namespace Darwin       { IResourceManager* CreateResourceManager(); }
namespace WireframeES1 { IRenderingEngine* CreateRenderingEngine(); }
namespace WireframeES2 { IRenderingEngine* CreateRenderingEngine(); }
namespace SolidES1     { IRenderingEngine* CreateRenderingEngine(); }
namespace SolidES1     { IRenderingEngine* CreateRenderingEngine(IResourceManager*); }
namespace SolidES2     { IRenderingEngine* CreateRenderingEngine(); }
namespace SolidES2     { IRenderingEngine* CreateRenderingEngine(IResourceManager*); }
namespace SolidGL2     { IRenderingEngine* CreateRenderingEngine(); }
namespace FacetedES1   { IRenderingEngine* CreateRenderingEngine(); }
namespace FacetedES2   { IRenderingEngine* CreateRenderingEngine(); }
namespace DepthViewer  { IRenderingEngine* CreateRenderingEngine(); }
namespace TexturedES1  { IRenderingEngine* CreateRenderingEngine(IResourceManager*); }
namespace TexturedES2  { IRenderingEngine* CreateRenderingEngine(IResourceManager*); }
namespace TexturedGL2  { IRenderingEngine* CreateRenderingEngine(IResourceManager*); }
