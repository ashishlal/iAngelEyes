#include "Interfaces.hpp"
#include "Common.hpp"
#include <string>
#include <map>
#include "ConstantsAndMacros.h"

#ifdef USE_ORIG_CODE
using namespace std;
using std::map;


struct strCmp {
	bool operator()( const char* s1, const char* s2 ) const {
        return strcmp( s1, s2 ) < 0;
	}
};

class ObjSurface : public ISurface {
public:
    ObjSurface(const string& name);
	ObjSurface(const string& name, int);
	ObjSurface();
	//~ObjSurface();
    int GetVertexCount() const;
    int GetLineIndexCount() const { return 0; }
    int GetTriangleIndexCount() const;
    void GenerateVertices(vector<float>& vertices, unsigned char flags) ;
    void GenerateLineIndices(vector<unsigned short>& indices) const {}
    void GenerateTriangleIndices(vector<unsigned short>& indices) const;
	void SetScreenPos(float x, float y) {m_screenpos.x=x; m_screenpos.y=y;};
	void SetScreenPos(float x, float y, float w, float z) {m_screenpos.x=x; m_screenpos.y=y;m_screenpos.z=z; m_screenpos.w=w;};
	vec4 GetScreenPos() {return m_screenpos;};
	vec4 GetBoundingSphere() {return m_boundingSphere;};
	void SetBoundingSphere(vec3 center, float radius) {m_boundingSphere.x = center.x;
		m_boundingSphere.y = center.y;
		m_boundingSphere.z = center.z;
		m_boundingSphere.w = radius;};
	int GetGroupCount() const;	
	void GetGroup() const;
	int GetTextureCoordsCount() const;
	void GetMaterialsFromMtlFile();
#if 1
	void GetGroupAndVerticesOLD(vector<Group *> *g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
									 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *);
#endif
	void GetGroupAndVertices(int index, vector<Group *> *g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
							 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *);
	void GetGroupMaterial(vector<Group *> *g);
	void InitVertices(int surfaceIndex);
private:
    string m_name;
    vector<ivec3> m_faces;
	mutable unsigned int  m_faceCount;
    mutable unsigned int m_vertexCount;
	mutable unsigned int m_groupCount;
	mutable unsigned int m_textureCoordsCount;
	mutable unsigned int m_numberOfVertices;
	mutable unsigned int m_numberOfFaces;
	float				*m_textureCoords;
	unsigned int		m_valuesPerCoord;	
#ifdef USE_ORIG_CODE
	mutable std::map < char *, Material *, strCmp > m_materials;
#endif
	mutable vector<Group *> m_groups;
	vector<string> m_vertexCombinations;
	vec3 *m_vertices;
	vec3 *m_vertexNormals;
	float *m_allTextureCoords;
	VertexTextureIndex *m_rootNode;
	vec4 m_screenpos;
	vec4 m_boundingSphere;
	char m_eolChar;
   // static const int MaxLineSize = 256;
};

#else

class ObjSurface : public ISurface {
public:
    
	ObjSurface();
	~ObjSurface() {};
    
	void SetScreenPos(float x, float y) {m_screenpos.x=x; m_screenpos.y=y;};
	void SetScreenPos(float x, float y, float w, float z) {m_screenpos.x=x; m_screenpos.y=y;m_screenpos.z=z; m_screenpos.w=w;};
	vec4 GetScreenPos() {return m_screenpos;};
	vec4 GetBoundingSphere() {return m_boundingSphere;};
	void SetBoundingSphere(vec3 center, float radius) {m_boundingSphere.x = center.x;
		m_boundingSphere.y = center.y;
		m_boundingSphere.z = center.z;
		m_boundingSphere.w = radius;};
	
#if 1
	void GetGroupAndVerticesOLD(vector<Group *> *g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
								vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *);

	void GetGroupAndVertices(int index, vector<Group *> *g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
							 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *, int);
	void GetGroupMaterial(vector<Group *> *g);
#else
	void GetGroupAndVerticesOLD(Group **g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
								vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *);
	
	void GetGroupAndVertices(int index, Group **g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
							 vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *);
	
#endif
	void InitVertices(int surfaceIndex);
	void SetSurfaceTypeIndex(int n) {m_surfaceType = n;};
private:
    string m_name;
#if 1
    vector<ivec3> m_faces;
#endif
	mutable unsigned int  m_faceCount;
    mutable unsigned int m_vertexCount;
	mutable unsigned int m_groupCount;
	mutable unsigned int m_textureCoordsCount;
	mutable unsigned int m_numberOfVertices;
	mutable unsigned int m_numberOfFaces;
	float				*m_textureCoords;
	unsigned int		m_valuesPerCoord;	

	//Group *m_groups[5];
	mutable vector<Group *> m_groups[SurfaceCount];
	vec3 *m_vertices;
	vec3 *m_vertexNormals;
	float *m_allTextureCoords;
	VertexTextureIndex *m_rootNode;
	vec4 m_screenpos;
	vec4 m_boundingSphere;
	char m_eolChar;
	int m_surfaceType;
	// static const int MaxLineSize = 256;
};

#endif
