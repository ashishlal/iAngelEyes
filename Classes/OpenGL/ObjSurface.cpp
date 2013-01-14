#include "ObjSurface.hpp"

#include <list>
#import <fstream>
#import <assert.h>
#import <iostream>

#include "ConstantsAndMacros.h"
#include "NTIMalloc.h"
#include "Eye111.h"

//#include "Common.hpp"
//#define USE_FAST_NORMALIZE

using namespace std;

#ifdef USE_ORIG_OBJ
typedef string::size_type (string::*find_t)(const string& delim, 
											string::size_type offset) const;

/// <summary>
/// Splits the string s on the given delimiter(s) and
/// returns a list of tokens without the delimiter(s)
/// </summary>
/// <param name=s>The string being split</param>
/// <param name=match>The delimiter(s) for splitting</param>
/// <param name=removeEmpty>Removes empty tokens from the list</param>
/// <param name=fullMatch>
/// True if the whole match string is a match, false
/// if any character in the match string is a match
/// </param>
/// <returns>A list of tokens</returns>
static vector<string> Split(const string& s,
					 const string& match,
					 bool removeEmpty=true,
					 bool fullMatch=true)
{
	vector<string> result;                 // return container for tokens
	string::size_type start = 0,           // starting position for searches
	skip = 1;            // positions to skip after a match
	find_t pfind = &string::find_first_of; // search algorithm for matches
	
	if (fullMatch)
	{
		// use the whole match string as a key
		// instead of individual characters
		// skip might be 0. see search loop comments
		skip = match.length();
		pfind = &string::find;
	}
	
	while (start != string::npos)
	{
		// get a complete range [start..end)
		string::size_type end = (s.*pfind)(match, start);
		
		// null strings always match in string::find, but
		// a skip of 0 causes infinite loops. pretend that
		// no tokens were found and extract the whole string
		if (skip == 0) end = string::npos;
		
		string token = s.substr(start, end - start);
		
		if (!(removeEmpty && token.empty()))
		{
			// extract the token and add it to the result list
			result.push_back(token);
		}
		
		// start the next range
		if ((start = end) != string::npos) start += skip;
	}
	
	return result;
}

static vector <float> GetFloatFromString(const string& s)
{
	std::string myL = s.substr(3);
	
	vector<string> vec = Split(myL, " ");
	vector<float> colorParts;
	vector<string>::iterator itr;
	int k=0;
	for (itr = vec.begin(); itr < vec.end(); itr++, k++)
	{
		colorParts.push_back(atof(vec[k].c_str()));
		
	}
	return colorParts;
}


#include "Common.hpp"
#define USE_FAST_NORMALIZE

static void calculateNormals(vec3 *vertices, unsigned int numberOfFaces, unsigned int numberOfVertices, vec3 **sNormals, 
					  vec3 **vNormals, vector<Group *> groups)
{
	vec3 *surfaceNormals;
	vec3 *vertexNormals;
	
	// Calculate surface normals and keep running sum of vertex normals
	//surfaceNormals = (vec3 *)calloc(numberOfFaces, sizeof(vec3));
	//vertexNormals = (vec3 *)calloc(numberOfVertices, sizeof(vec3));
#ifdef MALLOC_DEBUG
	// +++ lal: added 20 
	surfaceNormals = (vec3 *)NTIMalloc(((numberOfFaces) * sizeof(vec3)), (char *)__FILE__, __LINE__);
	memset(surfaceNormals, 0, (numberOfFaces ) * sizeof(vec3));
#else
	//surfaceNormals = (vec3 *)malloc(numberOfFaces * sizeof(vec3));
	//memset(surfaceNormals, 0, (numberOfFaces ) * sizeof(vec3));
	surfaceNormals = (vec3 *)calloc(numberOfFaces, sizeof(vec3));
#endif
	
	
	
#ifdef MALLOC_DEBUG
	// +++ lal: added 20
	vertexNormals = (vec3 *)NTIMalloc((numberOfVertices )* sizeof(vec3)), (char *)__FILE__, __LINE__);
	memset(vertexNormals, 0, (numberOfVertices) * sizeof(vec3));
#else
	//vertexNormals = (vec3 *)malloc((numberOfVertices) * sizeof(vec3));
	//memset(vertexNormals, 0, (numberOfVertices) * sizeof(vec3));
	vertexNormals = (vec3 *)calloc((numberOfVertices), sizeof(vec3));
#endif
	
	
	
	unsigned int index = 0;
	//unsigned int *facesUsedIn = (unsigned int *)calloc(numberOfVertices, sizeof(unsigned int)); // Keeps track of how many triangles any given vertex is used in
#ifdef MALLOC_DEBUG
	unsigned int *facesUsedIn = (unsigned int *)NTIMalloc(((numberOfVertices ) * sizeof(unsigned int)), (char *)__FILE__, __LINE__);
	memset(facesUsedIn, 0, (numberOfVertices) * sizeof(unsigned int));
#else
	//unsigned int *facesUsedIn = (unsigned int *)malloc((numberOfVertices) * sizeof(unsigned int));
	//memset(facesUsedIn, 0, (numberOfVertices) * sizeof(unsigned int));
	unsigned int *facesUsedIn = (unsigned int *)calloc((numberOfVertices), sizeof(unsigned int));
#endif
	
	
	for (int i = 0; i < groups.size(); i++)
	{
		Group oneGroup = *(groups[i]);
		for (int j = 0; j < oneGroup.numberOfFaces; j++)
		{
			Triangle3D triangle;
			triangle.v1 = Vector3DMake(vertices[oneGroup.faces[j].v1].x, 
									   vertices[oneGroup.faces[j].v1].y, 
									   vertices[oneGroup.faces[j].v1].z);
			
			triangle.v2 = Vector3DMake(vertices[oneGroup.faces[j].v2].x, 
									   vertices[oneGroup.faces[j].v2].y, 
									   vertices[oneGroup.faces[j].v2].z);
			
			triangle.v3 = Vector3DMake(vertices[oneGroup.faces[j].v3].x, 
									   vertices[oneGroup.faces[j].v3].y, 
									   vertices[oneGroup.faces[j].v3].z);
			
			Vector3D sN = Triangle3DCalculateSurfaceNormal(triangle);
			Vector3D vN1, vN2, vN3;
			vN1.x = vertexNormals[oneGroup.faces[j].v1].x;
			vN1.y = vertexNormals[oneGroup.faces[j].v1].y;
			vN1.z = vertexNormals[oneGroup.faces[j].v1].z;
			
			vN2.x = vertexNormals[oneGroup.faces[j].v2].x;
			vN2.y = vertexNormals[oneGroup.faces[j].v2].y;
			vN2.z = vertexNormals[oneGroup.faces[j].v2].z;
			
			vN3.x = vertexNormals[oneGroup.faces[j].v3].x;
			vN3.y = vertexNormals[oneGroup.faces[j].v3].y;
			vN3.z = vertexNormals[oneGroup.faces[j].v3].z;
		//	
//#ifdef USE_FAST_NORMALIZE
//			Vector3DFastNormalize(&sN);
////#else
			Vector3DNormalize(&sN);
//#endif
			vN1 = Vector3DAdd(sN, vN1);
			vN2 = Vector3DAdd(sN, vN2);
			vN3 = Vector3DAdd(sN, vN3);
			
			
			facesUsedIn[oneGroup.faces[j].v1]++;
			facesUsedIn[oneGroup.faces[j].v2]++;
			facesUsedIn[oneGroup.faces[j].v3]++;
			
			surfaceNormals[index] = vec3(sN.x, sN.y, sN.z);
			
			vertexNormals[oneGroup.faces[j].v1] = vec3(vN1.x, vN1.y, vN1.z);
			vertexNormals[oneGroup.faces[j].v2] = vec3(vN2.x, vN2.y, vN2.z);
			vertexNormals[oneGroup.faces[j].v3] = vec3(vN3.x, vN3.y, vN3.z);
			
			index++;
		}
	}
	
	cout << "vec3( " << vertexNormals[0].x <<  ", " << vertexNormals[0].y << " , " << vertexNormals[0].z << "); " << endl;
	
	// Loop through vertices again, dividing those that are used in multiple faces by the number of faces they are used in
	for (int i = 0; i < numberOfVertices; i++)
	{
		Vector3D vN;
		vN.x = vertexNormals[i].x;
		vN.y = vertexNormals[i].y;
		vN.z = vertexNormals[i].z;
		if(i == 0)
			cout << "vec3( " << vN.x <<  ", " << vN.y << " , " << vN.z << "); " << endl;
		if (facesUsedIn[i] > 1)
		{
			//vertexNormals[i].x /= facesUsedIn[i];
			//vertexNormals[i].y /= facesUsedIn[i];
			//vertexNormals[i].z /= facesUsedIn[i];
			
			vN.x /= facesUsedIn[i];
			vN.y /= facesUsedIn[i];
			vN.z /= facesUsedIn[i];
		}
		if(i == 0)
			cout << "vec3( " << vN.x <<  ", " << vN.y << " , " << vN.z << "); " << endl;
		
//#ifdef USE_FAST_NORMALIZE
//		Vector3DFastNormalize(&vN);
//#else
		Vector3DNormalize(&vN);
//#endif
		
		vertexNormals[i] = vec3(vN.x, vN.y, vN.z);
		if(i == 0)
			cout << "vec3( " << vertexNormals[0].x <<  ", " << vertexNormals[0].y << " , " <<  vertexNormals[0].z << "); " << endl;
	}
	//*sNormals = surfaceNormals;
	*sNormals = NULL;
	*vNormals = vertexNormals;
	//cout << "free " << __LINE__ << " " << facesUsedIn << endl;
#ifdef MALLOC_DEBUG
	
	NTIFree(surfaceNormals, (char *)__FILE__, __LINE__);
	NTIFree(facesUsedIn, (char *)__FILE__, __LINE__);
	//NTIMallocCheck((char *)__FILE__, __LINE__);
#else
	free(surfaceNormals);
	free(facesUsedIn);
#endif
}

static void	processOneVertex(VertexTextureIndex *rootNode, unsigned int vertexIndex, unsigned int vertexTextureIndex, 
							 unsigned int *vertexCount, vec3 *vertices, float  *allTextureCoords, float *textureCoords,
							 unsigned int componentsPerTextureCoord, unsigned short *faceVertexIndex)
{
	//NSLog(@"Processing Vertex: %d, Texture Index: %d", vertexIndex, vertexTextureIndex);
	bool alreadyExists = VertexTextureIndexContainsVertexIndex(rootNode, vertexIndex);
	VertexTextureIndex *vertexNode = VertexTextureIndexAddNode(rootNode, vertexIndex, vertexTextureIndex);
	if (vertexNode->actualVertex == UINT_MAX)
	{
		if ((!alreadyExists) || (rootNode == vertexNode))
		{
			
			vertexNode->actualVertex = vertexNode->originalVertex;
			
			// Wavefront's texture coord order is flip-flopped from what OpenGL expects
			for (int i = 0; i < componentsPerTextureCoord; i++)
				textureCoords[(vertexNode->actualVertex * componentsPerTextureCoord) + i] = allTextureCoords[(vertexNode->textureCoords * componentsPerTextureCoord) + componentsPerTextureCoord - (i+1)] ;
			
		}
		else
		{
			vertices[*vertexCount].x = vertices[vertexNode->originalVertex].x;
			vertices[*vertexCount].y = vertices[vertexNode->originalVertex].y;
			vertices[*vertexCount].z = vertices[vertexNode->originalVertex].z;
			vertexNode->actualVertex = *vertexCount;
			
			for (int i = 0; i < componentsPerTextureCoord; i++)
				textureCoords[(vertexNode->actualVertex * componentsPerTextureCoord) + i] = allTextureCoords[(vertexNode->textureCoords * componentsPerTextureCoord) + componentsPerTextureCoord - (i+1)];
			*vertexCount = *vertexCount + 1;
		}
	}
	*faceVertexIndex = (unsigned short)(vertexNode->actualVertex);
}


ObjSurface::ObjSurface(const string& name) :
	m_name(name),
	m_faceCount(0),
	m_vertexCount(0),
	m_groupCount(0),
	m_textureCoordsCount(0),
	m_numberOfVertices(0),
	m_numberOfFaces(0),
	m_textureCoords(NULL),
	m_valuesPerCoord(0),
	m_vertices(NULL),
	m_vertexNormals(NULL),
	m_allTextureCoords(NULL),
	m_rootNode(NULL),
	m_eolChar('\r')
{
#ifdef DEBUG
    m_faces.resize(GetTriangleIndexCount() / 3);
    ifstream objFile(m_name.c_str());
    vector<ivec3>::iterator face = m_faces.begin();
    while (objFile) {
        char c = objFile.get();
        if (c == 'f') {
            assert(face != m_faces.end() && "parse error");
            objFile >> face->x >> face->y >> face->z;
            *face++ -= ivec3(1, 1, 1);
        }
        objFile.ignore(MaxLineSize, '\n');
    }
    assert(face == m_faces.end() && "parse error");
	Material *m;
	m = new Material;
	strcpy(m->name,(char *)"default");
	m->diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	m->ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m->specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m->shininess = 65.0f;
	m_materials[(char *)"default"]= m;
#endif	
	//GetMaterialsFromMtlFile();
}

ObjSurface::ObjSurface(const string& name, int fileType=MAC_FILE) :
	m_name(name),
	m_faceCount(0),
	m_vertexCount(0),
	m_groupCount(0),
	m_textureCoordsCount(0),
	m_numberOfVertices(0),
	m_numberOfFaces(0),
	m_textureCoords(NULL),
	m_valuesPerCoord(0),
	m_vertices(NULL),
	m_vertexNormals(NULL),
	m_allTextureCoords(NULL),
	m_rootNode(NULL),
	m_eolChar('\r')
	
{
	
	if(fileType == UNIX_FILE) 
		m_eolChar = '\n';
	else if(fileType == WINDOWS_FILE) 
		m_eolChar = '\n'; // '\r\n'
	else if(fileType == MAC_FILE) 
		m_eolChar = '\r';
#ifdef NOT_NEEDED	
    m_faces.resize(GetTriangleIndexCount() / 3);
    ifstream objFile(m_name.c_str());
    vector<ivec3>::iterator face = m_faces.begin();
	string str, str1;
	vector<string> v;
	size_t found;
	
    while (objFile) {
       
        getline(objFile, str1, m_eolChar);
		
        if((found = str1.find("f ", 0)) != string::npos) {
			
            assert(face != m_faces.end() && "parse error");
							
			str = str1.substr(2);
			v = Split(str, " ");
			for(int i=0; i < v.size(); i++) {
					
				vector <string> v1 = Split(v[i], "/");
				if(i==0) face->x = atof(v1[0].c_str());
				if(i==1) face->y = atof(v1[0].c_str());
				if(i==2) face->z = atof(v1[0].c_str());
			}
            //cout << face->x << " " << face->y << " " << face->z << " " << n << endl;
            *face++ -= ivec3(1, 1, 1);
			
            
        }
        //objFile.ignore(256, m_eolChar);
    }
    assert(face == m_faces.end() && "parse error");
	
#ifdef MATERIAL_USED
	Material *m;
	m =  (Material *)malloc(sizeof(Material));
	strcpy(m->name,(char *)"default");
	m->diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	m->ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m->specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m->shininess = 65.0f;
	m_materials[(char *)"default"]= m;
	
    assert(face == m_faces.end() && "parse error");
#endif
#endif
}

ObjSurface::ObjSurface() :
	m_name("none"),
	m_faceCount(0),
	m_vertexCount(0),
	m_groupCount(0),
	m_textureCoordsCount(0),
	m_numberOfVertices(0),
	m_numberOfFaces(0),
	m_textureCoords(NULL),
	m_valuesPerCoord(0),
	m_vertices(NULL),
	m_vertexNormals(NULL),
	m_allTextureCoords(NULL),
	m_rootNode(NULL),
	m_eolChar('\r')

{
	
}

#ifdef USE_ORIG_OBJ

ObjSurface::~ObjSurface()
{
	m_faceCount =0 ;
	m_vertexCount =0; 
	m_groupCount =0;
	m_textureCoordsCount =0;
	m_numberOfVertices = 0;
	m_numberOfFaces = 0;
	
	if(m_textureCoords) {
		free((void *)m_textureCoords);
		m_textureCoords = NULL;
	}
	m_valuesPerCoord =0;
	if(m_vertices) {
		free((void *)m_vertices);
		m_vertices = NULL;
	}
	if(m_vertexNormals) {
		free((void *)m_vertexNormals);
		m_vertexNormals = NULL;
	}
	if(m_allTextureCoords) {
		free((void *)m_allTextureCoords);
		m_allTextureCoords = NULL;
	}
	if(m_rootNode != NULL) {
		VertexTextureIndexFree(m_rootNode);
		m_rootNode = NULL;
	}
	vector<Group *>::iterator itr;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++)
	{
		
		Group *group = (*itr);
		if(group->faces) 
			free((void *)(group->faces));
		group->faces = NULL;
		//if(group->material) free((void *)(group->material));
		free((void *)group);

	}
	m_groups.clear();
	
	
}
#endif

int ObjSurface::GetVertexCount() const
{
#ifdef USE_ORIG_OBJ
    if (m_vertexCount != 0)
        return m_vertexCount;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if (c == 'v')
            m_vertexCount++;
        objFile.ignore(MaxLineSize, m_eolChar);
    }
#endif
    return m_vertexCount;
}

int ObjSurface::GetTriangleIndexCount() const
{
#ifdef USE_ORIG_OBJ
    if (m_faceCount != 0)
        return m_faceCount * 3;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if (c == 'f') {
            m_faceCount++;
		}
        objFile.ignore(MaxLineSize, m_eolChar);
    }
#endif
    return m_faceCount * 3;
}

void ObjSurface::GenerateVertices(vector<float>& floats, unsigned char flags) 
{
#ifdef USE_ORIG_OBJ
    assert(flags == VertexFlagsNormals && "Unsupported flags.");

    struct Vertex {
        vec3 Position;
        vec3 Normal;
    };
	
	float min_x = 1000.0f, min_y = 1000.0f, min_z = 1000.0f;
	float max_x = -1000.0f, max_y = -1000.0f, max_z = -1000.0f;
    float radius=0.0f;
	
    // Read in the vertex positions and initialize lighting normals to (0, 0, 0).
    floats.resize(GetVertexCount() * 6);
    ifstream objFile(m_name.c_str());
    Vertex* vertex = (Vertex*) &floats[0];
    while (objFile) {
        char c = objFile.get();
        if (c == 'v') {
            vertex->Normal = vec3(0, 0, 0);
            vec3& position = (vertex++)->Position;
            objFile >> position.x >> position.y >> position.z;
			//fflush(objFile);
			(position.x < min_x)?min_x = position.x:min_x=min_x;
			(position.y < min_y)?min_y = position.y:min_y=min_y;
			(position.z < min_z)?min_z = position.z:min_z=min_z;
			
			(position.x > max_x)?max_x = position.x:max_x=max_x;
			(position.y > max_y)?max_y = position.y:max_y=max_y;
			(position.z > max_z)?max_z = position.z:max_z=max_z;
			
        }
        objFile.ignore(MaxLineSize, m_eolChar);
    }

	vec3 center((min_x + max_x)/2.0f, (min_y + max_y)/2.0f, (min_z + max_z)/2.0f);
	radius = sqrt(
				  ((max_x - center.x) * (max_x - center.x)) +
				  ((max_y - center.y) * (max_y - center.y)) +
				  ((max_z - center.z) * (max_z - center.z))
				  );
#ifdef DEBUG
	printf("%.6f %.6f %.6f %.6f %.6f %.6f\n", min_x, min_y, min_z, max_x, max_y, max_z);
	printf("%.6f %.6f %.6f %.6f\n", center.x, center.y, center.z, radius);
#endif	
	SetBoundingSphere(center, radius);
	
    vertex = (Vertex*) &floats[0];
    for (size_t faceIndex = 0; faceIndex < m_faces.size(); ++faceIndex) {
        ivec3 face = m_faces[faceIndex];

        // Compute the facet normal.
        vec3 a = vertex[face.x].Position;
        vec3 b = vertex[face.y].Position;
        vec3 c = vertex[face.z].Position;
        vec3 facetNormal = (b - a).Cross(c - a);

        // Add the facet normal to the lighting normal of each adjoining vertex.
        vertex[face.x].Normal += facetNormal;
        vertex[face.y].Normal += facetNormal;
        vertex[face.z].Normal += facetNormal;
    }

    // Normalize the normals.
    for (int v = 0; v < GetVertexCount(); ++v)
        vertex[v].Normal.Normalize();
#endif
}

void ObjSurface::GenerateTriangleIndices(vector<unsigned short>& indices) const
{
#ifdef USE_ORIG_OBJ
    indices.resize(GetTriangleIndexCount());
    vector<unsigned short>::iterator index = indices.begin();
    for (vector<ivec3>::const_iterator f = m_faces.begin(); f != m_faces.end(); ++f) {
        *index++ = f->x;
        *index++ = f->y;
        *index++ = f->z;
    }
#endif
}

int ObjSurface::GetGroupCount() const
{
#ifdef USE_ORIG_OBJ
    if (m_groupCount != 0)
        return m_groupCount;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if (c == 'g') m_groupCount++;
        objFile.ignore(MaxLineSize, m_eolChar);
    }
#endif
    return m_groupCount;
}

void ObjSurface::GetGroup() const
{
#ifdef USE_ORIG_OBJ
    if (m_groupCount == 0) return ;
    
    ifstream objFile(m_name.c_str());
	vector<std::string> lines;
	
	size_t found;
	unsigned int num_lines=0;
    while (objFile) {
        
		getline(objFile, lines[num_lines], m_eolChar);
		num_lines++;
	} 
	for(int i=0; i < num_lines; i++) {
		std::string line = lines[i];
		std::string nextLine = NULL;
		std::string materialName = NULL;
		
		if((found = line.find("g", 0)) != string::npos) {
			
			std::string groupName = line.substr(2);
			unsigned int counter = i+1;
			unsigned int currentGroupFaceCount = 0;
			while(counter < num_lines) {
				nextLine = lines[i+1];
				if((found = nextLine.find("usemtl", 0)) != string::npos) {
					materialName = nextLine.substr(7);
				}
				else if ((found = nextLine.find("f", 0)) != string::npos)
				{
					// TODO: Loook for quads and double-increment
					currentGroupFaceCount ++;
				}
				else if ((found = nextLine.find("g", 0)) != string::npos)
					break;
			}
#ifdef MATERIAL_USED
			Material *m;
			m= m_materials[(char *)materialName.c_str()];
			if(m == NULL) m = m_materials[(char *)"default"];
#endif
#ifdef MALLOC_DEBUG
			Group *currentGroup = (Group *)NTIMalloc(sizeof( Group), (char *)__FILE__, __LINE__);
#else
			Group *currentGroup = (Group *)malloc(sizeof( Group));
#endif
			memset(currentGroup, 0, sizeof(Group));
			//cout << "malloc " << __LINE__ << " " << currentGroup << endl;
			currentGroup->oneTexture=false;
			strcpy(currentGroup->name,(char *)(groupName.c_str()));
			currentGroup->numberOfFaces = currentGroupFaceCount;
			//currentGroup->material = m;
			currentGroup->material = NULL;
			m_groups.push_back(currentGroup);
		}
	}
#endif
	return;
}


int ObjSurface::GetTextureCoordsCount() const
{
#ifdef USE_ORIG_OBJ
	int valuesPerCoord = 2; //assume 2
    if (m_textureCoordsCount != 0)
        return m_textureCoordsCount * valuesPerCoord;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
		char c1 = objFile.get();
		if((c == 'v') && (c1 == 't'))
        //if (c == 'vt')
			m_textureCoordsCount++;
        objFile.ignore(MaxLineSize, '\n');
    }
    return m_textureCoordsCount * valuesPerCoord;
#else
	return 2;
#endif
}

void ObjSurface::GetMaterialsFromMtlFile()
{
#ifdef USE_ORIG_OBJ
	vector<string> vec1 = Split(m_name, "/");

	string objName = vec1[vec1.size() -1];
	
	vector <string> vec = Split(objName, ".");
	string mtlName;
	for(unsigned int v=0; v < vec1.size()-1; v++)
	{
		mtlName = mtlName.append(vec1[v]);
		mtlName = mtlName.append("/");
	}
	mtlName = mtlName.append(vec[0]);
	mtlName = mtlName.append(".mtl");
	cout << "objName = " << objName << endl;
	cout << "mtlname = " << mtlName << endl;
	ifstream mtlFile(mtlName.c_str());
	
	vector<std::string> lines;
	Material *m;
	size_t found;
	int num_lines=0;
	//char m_eolChar = '\r';
	
	std::string str;
    while (std::getline(mtlFile, str, '\r')) {
		lines.push_back(str);
		num_lines++;
	}
	
	for(int i=0; i < num_lines; i++) {
		std::string line = lines[i];
		if((found = line.find("newmtl", 0)) != string::npos) {
			
			// Determine start of next material
			int mtlEnd = -1;
			for (int j = i+1; j < num_lines; j++)
			{
				std::string innerLine = lines[j];
				if((found = innerLine.find("newmtl", 0)) != string::npos) 
				{
					mtlEnd = j-1;
					
					break;
				}
				
			}
			if (mtlEnd == -1)
				mtlEnd = num_lines-1;
			m = (Material *) malloc(sizeof(Material));
			//cout << "malloc " << __LINE__ << " " << m << endl;
			for (int j = i; j <= mtlEnd; j++)
			{
				std::string parseline = lines[j];
				// ignore Ni, d, and illum, and texture - at least for now
				if(((found = parseline.find("newmtl ", 0)) != string::npos) && (found == 0)) {
					strcpy((char *)(m->name), (char *)(parseline.substr(7).c_str()));
					//cout << "n = " << m->name << endl;
				}
				else if (((found = parseline.find("Ns ", 0)) != string::npos) && (found == 0)) {
					m->shininess = (float)atof((parseline.substr(3).c_str()));
					//cout << "sh = " << m->shininess << endl;
				}
				else if (((found = parseline.find("Ka spectral", 0)) != string::npos) && (found == 0))
				{
					
				}
				else if (((found = parseline.find("Ka ", 0)) != string::npos)  && (found == 0))
				{
					vector<float> colorParts = GetFloatFromString(parseline);
					m->ambient = vec4(colorParts[0], colorParts[1], colorParts[2], 1.0f);
					//cout << "amb =" << colorParts[0] << " " << colorParts[1] << " " <<
					//colorParts[2] << endl;
					
				}
				
				else if (((found = parseline.find("Kd ", 0)) != string::npos) && (found == 0))
				{
					vector<float> colorParts = GetFloatFromString(parseline);
					//cout << " p = " << parseline << endl;
					m->diffuse = vec4(colorParts[0], colorParts[1], colorParts[2], 1.0f);
					//cout << "diff =" << colorParts[0] << " " << colorParts[1] << " " <<
					//colorParts[2] << endl;
				}
				
				else if (((found = parseline.find("Ks ", 0)) != string::npos) && (found == 0))
				{
					vector<float> colorParts = GetFloatFromString(parseline);
					m->specular = vec4(colorParts[0], colorParts[1], colorParts[2], 1.0f);
					//cout << "spec =" << colorParts[0] << " " << colorParts[1] << " " <<
					//" " << colorParts[2] << endl;
				}
				else if (((found = parseline.find("Ni ", 0)) != string::npos) && (found == 0))
				{
					string myL = parseline.substr(3);
					float opticalDensity = atof(myL.c_str());
					m->opticalDensity = opticalDensity;
					//cout << "Ni = " << m->opticalDensity << endl;
				}
				else if (((found = parseline.find("d ", 0)) != string::npos) && (found == 0))
				{
					string myL = parseline.substr(2);
					float dissolve = atof(myL.c_str());
					m->dissolve = dissolve;
					//cout << "d = " << m->dissolve << endl;
				}
				else if (((found = parseline.find("illum ", 0)) != string::npos) && (found == 0))
				{
					string myL = parseline.substr(6);
					float illumination = atof(myL.c_str());
					m->illumination = illumination;
					//cout << "illum = " << m->illumination << endl;
				}
				else if (((found = parseline.find("map_Kd ", 0)) != string::npos) && (found == 0))
				{
					//glEnableClientState(GL_TEXTURE);
					
					std::string texName = parseline.substr(7);
					vector<string> vec = Split(texName, ".");
					std::string baseName = vec[0];
					std::string baseType = vec[1];
					std::string textureFilename = texName;
					
					Texture *theTex = (Texture *)malloc(sizeof( Texture));
					strcpy(theTex->filename,(char *)(baseName.c_str()));
					strcpy(theTex->filetype,(char *)(baseType.c_str()));
					
					//cout << "tex fname="  << theTex->filename << " type " << theTex->filetype << endl;
					// TODO: Look for PVRT file
					//OpenGLTexture3D *theTex = [[OpenGLTexture3D alloc] initWithFilename:textureFilename width:width height:height];
					m->texture = theTex;
					
				}
				
			}
#if 0
			cout << m->name << endl;
			cout << " mat22 = " << m->name << " " << 
			m->diffuse.x << " " << 
			m->diffuse.y << " " <<
			m->diffuse.z << " " <<
			m->diffuse.w << " " <<
			m->ambient.x << " " << 
			m->ambient.y << " " <<
			m->ambient.z << " " <<
			m->ambient.w << " " <<
			m->specular.x << " " << 
			m->specular.y << " " <<
			m->specular.z << " " <<
			m->specular.w << " " <<
			m->shininess << " " <<  endl;	
#endif
			m_materials[(char *)(m->name)] = m;
			
			i = mtlEnd;
			
		}
		
    }
#endif
}

template <typename Pair>
struct select1st
{
	typedef Pair argument_type ;
	typedef typename Pair::first_type result_type ;
	
	const result_type &
	operator()(const argument_type &p) const
	{
		return p.first ;
	}
} ;

template <typename Pair>
struct select2nd
{
	typedef Pair argument_type ;
	typedef typename Pair::second_type result_type ;
	
	const result_type &
	operator()(const argument_type &p) const
	{
		return p.second ;
	}
} ;


void ObjSurface::GetGroupAndVerticesOLD(vector<Group *> *g, vec3 **vert, unsigned int *nVertices, unsigned int *nFaces,
									vec3 **sNormals, vec3 **vNormals, float **texCoords, 
									 unsigned int *vPerCoord, unsigned int *texCoordsCount)
{
#ifdef USE_ORIG_OBJ
	
	//size_t m_textureCoordsCount =0;
	
	//float				*m_textureCoords = NULL;
	//unsigned int		m_valuesPerCoord=0;	
	//std::map < char *, Material *, strCmp > m_materials;
	
	vector <string> lines;
	//vector <string> dupLines;
	//vector <string> dupdupLines;
	
	size_t found;
	unsigned int num_obj_lines=0;
	
	//unsigned int		numberOfVertices=0;
	//unsigned int		numberOfFaces=0;			// Total faces in all groups
	
	// length = numberOfFaces (*3 vertices per triangle);
	
	//vec3 *vertices = NULL;
	bool firstTextureCoords = true;
	bool oneTexture = true;
	
	//unsigned int vertexCount = 0, groupCount=0, faceCount=0;
	m_textureCoordsCount=0;

#ifdef MATERIALS_USED
	Material *m;
	m = (Material *) malloc(sizeof(Material));
	strcpy(m->name, (char *)"default");
	m->diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	m->ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m->specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m->shininess = 65.0f;
	m_materials[(char *)"default"]= m;
#endif	
	
	ifstream objFile(m_name.c_str());
	std::string str;
    while (std::getline(objFile, str, m_eolChar)) {
		lines.push_back(str);
		//dupLines.push_back(str);
		//dupdupLines.push_back(str);
		num_obj_lines++;
	}
	objFile.close();
	int i=0;
	m_valuesPerCoord=2;
	
	for ( vector<string>::iterator line_itr = lines.begin(); line_itr != lines.end(); line_itr++) {
	
		string line = (*line_itr);
		
		if(((found = line.find("v ", 0)) != string::npos) && (found == 0))
			m_vertexCount++;
		else if(((found = line.find("vt ", 0)) != string::npos) && (found == 0))
		{
			m_textureCoordsCount++;
			if (firstTextureCoords)
			{
				firstTextureCoords = false;
				m_valuesPerCoord=0;
				std::string texLine = line.substr(3);
				vector<string> vec = Split(texLine, " ");
				vector<string>::iterator numCoords;
				for (numCoords = vec.begin(); numCoords != vec.end(); numCoords++)
					m_valuesPerCoord++;
				//cout << "textLine = " << texLine << " vpc = " << m_valuesPerCoord << endl;
			}
		}
		else if(((found = line.find("m", 0)) != string::npos) && (found == 0))
		{
			if(!oneTexture) {
				GetMaterialsFromMtlFile();
				map< char *, Material *>::iterator matItr;
				for(matItr = m_materials.begin(); matItr != m_materials.end(); matItr++) {
				
#ifdef DEBUG				
					Material * m = (*matItr).second;
					//cout << (*matItr).first << endl;

					cout << " mat11 = " << m->name << endl;
					cout << " mat11 = " << m->name << " " << 
					m->diffuse.x << " " << 
					m->diffuse.y << " " <<
					m->diffuse.z << " " <<
					m->diffuse.w << " " <<
					m->ambient.x << " " << 
					m->ambient.y << " " <<
					m->ambient.z << " " <<
					m->ambient.w << " " <<
					m->specular.x << " " << 
					m->specular.y << " " <<
					m->specular.z << " " <<
					m->specular.w << " " <<
					m->shininess << " " <<  endl;	
#endif
				}
			}

		}
		else if(((found = line.find("g ", 0)) != string::npos) && (found == 0))
			m_groupCount++;
		else if(((found = line.find("f ", 0)) != string::npos) && (found == 0))
		{
			m_faceCount++;
			std::string faceLine = line.substr(2);
			vector<std::string>faces = Split(faceLine, " ");
			//cout << "sz = " << faces.size() << " cnt = " << faceCount << endl;
			vector<string>::iterator oneFace;
			for (oneFace = faces.begin(); oneFace != faces.end(); oneFace++)
			{
#if 1
				vector<std::string>faceParts = Split(*oneFace, "/");
				
				string faceKey = faceParts[0];
				faceKey.append("/");
				if(faceParts.size() > 0) 
					faceKey.append(faceParts[1]);
				else 
					faceKey.append("0");
				
				
				
				vector<string>::iterator itr;
				for (itr = m_vertexCombinations.begin(); itr != m_vertexCombinations.end(); itr++)
					if(*itr == faceKey)	break;				
				if (itr == m_vertexCombinations.end()) m_vertexCombinations.push_back(faceKey)
					;
#else
				
				vector<string>::iterator itr;
				for (itr = m_vertexCombinations.begin(); itr != m_vertexCombinations.end(); itr++)
					if(*itr == *oneFace)	break;				
				if (itr == m_vertexCombinations.end()) {
					m_vertexCombinations.push_back(*oneFace);
				}
#endif					
			}
		}
		i++;
	}
	
	//cout << "m_textureCoordsCount1 = " << m_textureCoordsCount << endl;
#ifdef MALLOC_DEBUG
	//cout << "vc sz= " << vertexCombinations.size() << endl;
	m_vertices = (vec3 *)NTIMalloc((sizeof(vec3) *  (m_vertexCombinations.size() + 20)), (char *)__FILE__, __LINE__);
	memset(m_vertices, 0, sizeof(vec3) *  (m_vertexCombinations.size() + 20));
	
	m_allTextureCoords = 
	(float *)NTIMalloc((sizeof(float) *  (m_textureCoordsCount + 80) * m_valuesPerCoord), (char *)__FILE__, __LINE__);
	memset(m_allTextureCoords, 0, sizeof(float) *  (m_textureCoordsCount + 80) * m_valuesPerCoord);
	
	m_textureCoords = 
	(m_textureCoordsCount > 0) ?  
	(float *)NTIMalloc((sizeof(float) * m_valuesPerCoord * (m_vertexCombinations.size() + 30)), (char *)__FILE__, __LINE__) : 
	NULL;
	memset(m_textureCoords, 0, sizeof(float) *  (m_vertexCombinations.size() + 30) * m_valuesPerCoord);
	
	//NTIMallocCheck((char *)__FILE__, __LINE__);
#else
	m_vertices = (vec3 *)malloc(sizeof(vec3) *  (m_vertexCombinations.size() + 10));
	memset(m_vertices, 0, sizeof(vec3) *  (m_vertexCombinations.size() + 10));
	//cout << "malloc " << __LINE__ << " " << vertices << endl;
	
	//cout << "m_textureCoordsCount = " << m_textureCoordsCount << endl;
	if(m_textureCoordsCount) {
		m_allTextureCoords = 
			(float *)malloc(sizeof(float) *  (m_textureCoordsCount + 10) * m_valuesPerCoord);
		memset(m_allTextureCoords, 0, sizeof(float) *  (m_textureCoordsCount + 10) * m_valuesPerCoord);
		if(m_valuesPerCoord != 2) cout << "!!!!!! m_valPerC = " << m_valuesPerCoord << endl;
		//cout << "malloc " << __LINE__ << " " << allTextureCoords << endl;
	}
	m_textureCoords = (float *)malloc(sizeof(float) * m_valuesPerCoord * (m_vertexCombinations.size() + 10));
	memset(m_textureCoords, 0, sizeof(float) *  (m_vertexCombinations.size() + 10) * m_valuesPerCoord);
	//cout << "malloc " << __LINE__ << " " << m_textureCoords << endl;
#endif	
	// Store the counts
	m_numberOfFaces = m_faceCount;
	m_numberOfVertices = m_vertexCombinations.size();
	unsigned int allTextureCoordsCount = 0;
	m_textureCoordsCount = 0;
	unsigned int groupFaceCount = 0;
	unsigned int groupCoordCount = 0;
	// Reuse our count variables for second time through
	m_vertexCount = 0;
	m_faceCount = 0;
	Group *currentGroup = NULL;
	//unsigned int lineNum = 0;
	bool usingGroups = true;
	vector <Face> f;
	
	int lineNum=0;
	float min_x = 1000.0f, min_y = 1000.0f, min_z = 1000.0f;
	float max_x = -1000.0f, max_y = -1000.0f, max_z = -1000.0f;
    float radius=0.0f;
	
//	for ( vector<string>::iterator line_itr = dupLines.begin(); line_itr != dupLines.end(); line_itr++) {
	for ( vector<string>::iterator line_itr = lines.begin(); line_itr != lines.end(); line_itr++) {	
		string line = (*line_itr);
		if(((found = line.find("v ", 0)) != string::npos) && (found == 0))
		{
			//cout << line << endl;
			std::string lineTrunc = line.substr(2);
			//cout << lineTrunc << endl;
			vector<string> lineVertices = Split(lineTrunc, " ");
			
			m_vertices[m_vertexCount].x = atof(lineVertices[0].c_str());
			m_vertices[m_vertexCount].y = atof(lineVertices[1].c_str());
			m_vertices[m_vertexCount].z = atof(lineVertices[2].c_str());
			// Ignore weight if it exists..
			//cout << m_vertices[vertexCount].x << " " << m_vertices[vertexCount].y << " " << m_vertices[vertexCount].z << " " << vertexCount << endl;
			
			if(m_vertices[m_vertexCount].x < min_x) min_x = m_vertices[m_vertexCount].x;
			if(m_vertices[m_vertexCount].y < min_y) min_y = m_vertices[m_vertexCount].y;
			if(m_vertices[m_vertexCount].z < min_z) min_z = m_vertices[m_vertexCount].z;
			
			if(m_vertices[m_vertexCount].x > max_x) max_x = m_vertices[m_vertexCount].x;
			if(m_vertices[m_vertexCount].y > max_y) max_y = m_vertices[m_vertexCount].y;
			if(m_vertices[m_vertexCount].z > max_z) max_z = m_vertices[m_vertexCount].z;
			
			m_vertexCount++;
			//NTIMallocCheck((char *)__FILE__, __LINE__);
			
			
		}
		else if(((found = line.find("vt ", 0)) != string::npos) && (found == 0))
		{
			string lineTrunc = line.substr(3);
			vector<string> lineCoords = Split(lineTrunc, " ");
			
			vector<string>::iterator oneCoord;
			for (oneCoord = lineCoords.begin(); oneCoord != lineCoords.end(); oneCoord++)
			{
				m_allTextureCoords[allTextureCoordsCount] = atof((*oneCoord).c_str());
				allTextureCoordsCount++;
			}
			
			// Ignore weight if it exists..
			m_textureCoordsCount++;
		}
		else if(((found = line.find("g ", 0)) != string::npos) && (found == 0))
		{
			string groupName = line.substr(2);
			unsigned int counter = lineNum+1;
			unsigned int currentGroupFaceCount=0;
			string materialName;

			
			//for (vector<string>::iterator ctr = dupdupLines.begin() + counter; ctr != dupdupLines.end(); ctr++) 
			for (vector<string>::iterator ctr = lines.begin() + counter; ctr != lines.end(); ctr++)
			{
				string nextLine = (*ctr);
				
				if(((found = nextLine.find("usemtl ", 0)) != string::npos)  && (found == 0))
					materialName = nextLine.substr(7);
				else if(((found = nextLine.find("f ", 0)) != string::npos)  && (found == 0))
				{
					// TODO: Loook for quads and double-increment
					currentGroupFaceCount ++;
				}
				else if (((found = nextLine.find("g ", 0)) != string::npos)  && (found == 0))
					break;
			}
			
			
			Material *material = NULL;
#ifdef MALLOC_DEBUG
			currentGroup = (Group *)NTIMalloc(sizeof(Group), (char *)__FILE__, __LINE__);
#else
			currentGroup = (Group *)malloc(sizeof(Group) );
			memset(currentGroup, 0, sizeof(Group));
#endif
			//cout << "malloc " << __LINE__ << " " << currentGroup << endl;
			currentGroup->oneTexture=true;
			if(!currentGroup->oneTexture) {
				material = m_materials[(char *)(materialName.c_str())] ;
				if (material == NULL)
					material = m_materials[(char *)"default"];
						
			}
			
			strcpy(currentGroup->name,(char *)(groupName.c_str()));
#ifdef MALLOC_DEBUG
			currentGroup->faces = (Face *)NTIMalloc((sizeof(Face) * (currentGroupFaceCount)), (char *)__FILE__, __LINE__);
#else
			currentGroup->faces = (Face *)malloc(sizeof(Face) * (currentGroupFaceCount));
#endif
			memset(currentGroup->faces, 0, sizeof(Face) * (currentGroupFaceCount));
			//cout << "malloc " << __LINE__ << " " << currentGroup->faces << endl;
			currentGroup->numberOfFaces = currentGroupFaceCount;
			//currentGroup->material = material;	
			currentGroup->material = NULL;
			m_groups.push_back(currentGroup);
			
			groupFaceCount = 0;
			groupCoordCount = 0;
			
		}
		else if(((found = line.find("usemtl ", 0)) != string::npos) && (found == 0))
		{
			string materialKey = line.substr(7);
#ifdef MATERIAL_USED
			currentGroup->material = m_materials[(char *)(materialKey.c_str())];
#else
			currentGroup->material = NULL;
#endif

		}
		else if(((found = line.find("f ", 0)) != string::npos) && (found == 0))
		{
			string lineTrunc = line.substr(2);
			vector<string> faceIndexGroups = Split(lineTrunc, " ");
			
			// If no groups in file, create one group that has all the vertices and uses the default material
			if (currentGroup == NULL)
			{
				Material *tempMaterial = NULL;
				vector< char *> materialKeys;
				
				
				materialKeys.reserve(m_materials.size());
				std::transform(m_materials.begin(), m_materials.end(), std::back_inserter(materialKeys),
							   select1st<std::map< char *, Material *>::value_type>()) ;
				
				if (materialKeys.size() == 2)
				{
					vector< char *>::iterator key;
					string def("default");
					
					for(key=materialKeys.begin(); key != materialKeys.end(); key++)
						if(strcmp(*key, def.c_str()))
							tempMaterial = m_materials[*key];
				}
				if (tempMaterial == NULL)
					tempMaterial = m_materials[(char *)"default"];
#ifdef MALLOC_DEBUG				
				currentGroup = (Group *)NTIMalloc(sizeof(Group), (char *)__FILE__, __LINE__);
#else
				currentGroup = (Group *)malloc(sizeof(Group));
#endif
				memset(currentGroup, 0, sizeof(Group));
				//cout << "malloc " << __LINE__ << " " << currentGroup << endl;
				currentGroup->oneTexture=true;
				string s("default");
				strcpy(currentGroup->name, (char *)"default");
#ifdef MALLOC_DEBUG
				currentGroup->faces = (Face *)NTIMalloc((sizeof(Face) * (m_numberOfFaces )), (char *)__FILE__, __LINE__);
#else
				currentGroup->faces = (Face *)malloc((sizeof(Face) * (m_numberOfFaces)));
#endif
				memset(currentGroup->faces, 0, (sizeof(Face) * (m_numberOfFaces)));
				currentGroup->numberOfFaces = m_numberOfFaces;
#ifdef MATERIAL_USED
				currentGroup->material = tempMaterial;
#else
				currentGroup->material = NULL;
#endif
				
				m_groups.push_back(currentGroup);
			
				usingGroups = false;
			}
			
			//cout << "vc0 = " << vertexCount << endl;
			//cout << "gfc = " << groupFaceCount << " " << currentGroup << endl;
			// TODO: Look for quads and build two triangles
			vector<string> vertex1Parts = Split(faceIndexGroups[0], "/");
			unsigned int vertex1Index = (unsigned int)(atoi(vertex1Parts[kGroupIndexVertex].c_str())) - 1;
			unsigned int vertex1TextureIndex=0;
			if(vertex1Parts.size() > 1)
				vertex1TextureIndex = (int)(atoi(vertex1Parts[kGroupIndexTextureCoordIndex].c_str())) -1;
			if (m_rootNode == NULL)
				m_rootNode =  VertexTextureIndexMake(vertex1Index, vertex1TextureIndex, UINT_MAX);
			//NTIMallocCheck((char *)__FILE__, __LINE__);
			processOneVertex(m_rootNode, vertex1Index, vertex1TextureIndex, &m_vertexCount, m_vertices, 
							 m_allTextureCoords, m_textureCoords, m_valuesPerCoord, 
							 // &v1);
							 &(currentGroup->faces[groupFaceCount].v1));
			//cout << "vc1 = " << vertexCount << endl;
			//NTIMallocCheck((char *)__FILE__, __LINE__);
			vector<string> vertex2Parts = Split(faceIndexGroups[1], "/");
			processOneVertex(m_rootNode, atoi(vertex2Parts[kGroupIndexVertex].c_str())-1, 
							 (vertex2Parts.size() > 1) ? atoi(vertex2Parts[kGroupIndexTextureCoordIndex].c_str())-1 : 0, 
							 &m_vertexCount, m_vertices, m_allTextureCoords, m_textureCoords, m_valuesPerCoord, 
							 //&v2);
							 &(currentGroup->faces[groupFaceCount].v2));
			
			//cout << "vc2 = " << vertexCount << endl;
			//NTIMallocCheck((char *)__FILE__, __LINE__);
			vector<string> vertex3Parts = Split(faceIndexGroups[2], "/");
			processOneVertex(m_rootNode, 
							 atoi(vertex3Parts[kGroupIndexVertex].c_str())-1, 
							 (vertex3Parts.size() > 1) ? atoi(vertex3Parts[kGroupIndexTextureCoordIndex].c_str())-1 : 0,
							 &m_vertexCount, m_vertices, m_allTextureCoords, m_textureCoords, m_valuesPerCoord, 
							 //&v3);
							 &(currentGroup->faces[groupFaceCount].v3));
			
			//cout << "vc3 = " << vertexCount << endl;
			//NTIMallocCheck((char *)__FILE__, __LINE__);
#ifdef DEBUG
			cout << currentGroup->faces[groupFaceCount].v1 << " " <<
			currentGroup->faces[groupFaceCount].v2 << " " <<
			currentGroup->faces[groupFaceCount].v3 << endl;
#endif
			
			m_faceCount++;
			groupFaceCount++;
			//currentGroup->numberOfFaces = groupFaceCount;
			//cout << "gfc = " << groupFaceCount << endl;
			
			//	if((groupFaceCount % 5000) == 0) {
			// TBD
			//}
		}
		
		
		
		//i++;
		lineNum++;
		
	}
	
	vec3 center((min_x + max_x)/2.0f, (min_y + max_y)/2.0f, (min_z + max_z)/2.0f);
	radius = sqrt(
				  ((max_x - center.x) * (max_x - center.x)) +
				  ((max_y - center.y) * (max_y - center.y)) +
				  ((max_z - center.z) * (max_z - center.z))
				  );
#ifdef DEBUG
	printf("%.6f %.6f %.6f %.6f %.6f %.6f\n", min_x, min_y, min_z, max_x, max_y, max_z);
	printf("%.6f %.6f %.6f %.6f\n", center.x, center.y, center.z, radius);
#endif	
	
	calculateNormals(m_vertices, m_numberOfFaces, m_numberOfVertices, sNormals, &m_vertexNormals, m_groups);

	
	vector<Group *>::iterator itr;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++)
	{
		(*g).push_back(*itr);
	
		
	}

	SetBoundingSphere(center, radius);

	//NTIMallocCheck((char *)__FILE__, __LINE__);
	*vert = m_vertices;
	*nVertices = m_numberOfVertices;
	*vNormals = m_vertexNormals;
	*nFaces = m_numberOfFaces;
	
	*texCoords = m_textureCoords;
	*vPerCoord = m_valuesPerCoord;
	*texCoordsCount = m_textureCoordsCount;
	//*sNormals = (vec3 *)m_allTextureCoords;
	//cout << *nFaces << " " << *nVertices << " " << *vPerCoord << " " << m_valuesPerCoord << endl;
	//cout << "num g = " << m_groups.size() << endl;
	
#if 0	
	//cout << "nv = " << *nVertices << endl;
	for(int v=0; v < *nVertices; v++)
	{
		cout << "v" << v << " " << m_vertices[v].x << " " << m_vertices[v].y << " " << m_vertices[v].z << endl;
		cout << "vn" << v << " " << (*vNormals)[v].x << " " << (*vNormals)[v].y << " " << (*vNormals)[v].z << endl;
	}
	cout << "---------------------------------------------------------------------------" << endl;
	for(int v=0; v < *nFaces; v++)
	{
		//cout << vertices[v].x << " " << vertices[v].y << " " << vertices[v].z << endl;
		//cout << (*sNormals)[v].x << " " << (*sNormals)[v].y << " " << (*sNormals)[v].z << endl;
	}
#endif	
	
	
#if 0
	// ++lal:crashes here with malloc_error_break;
	if (m_allTextureCoords) {
#ifdef MALLOC_DEBUG
		NTIFree(allTextureCoords, (char *)__FILE__, __LINE__);
#else
		free(m_allTextureCoords);
		m_allTextureCoords = NULL;
#endif
	}
#endif	
	
	
	//NTIMallocCheck((char *)__FILE__, __LINE__);
#endif
}


void ObjSurface::GetGroupMaterial(vector<Group *> *g)
{
#ifdef USE_ORIG_OBJ
	vector<Group *>::iterator itr;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++)
	{
		(*g).push_back(*itr);
	}
#endif
}

void ObjSurface::GetGroupAndVertices(int index, vector<Group *> *g, vec3 **vert, unsigned int *nVertices, unsigned int *nFaces,
										vec3 **sNormals, vec3 **vNormals, float **texCoords, 
										unsigned int *vPerCoord, unsigned int *texCoordsCount)
{
#if 0
	InitVertices(index);
	*vert = m_vertices;
	*nVertices = m_numberOfVertices;
	*vNormals = m_vertexNormals;
	*nFaces = m_numberOfFaces;
	
	*texCoords = m_textureCoords;
	*vPerCoord = m_valuesPerCoord;
	*texCoordsCount = m_textureCoordsCount;
	
	vector<Group *>::iterator itr;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++)
	{
		(*g).push_back(*itr);
		
		
	}
#endif
}



void ObjSurface::InitVertices(int surfaceIndex)
{
#if 0
	vector<Group *> ::iterator itr;
	Group *group;

	if(m_groups.size() == 0 ) {
		m_groups.clear();
		group = (Group *)malloc(sizeof(Group));
		memset(group, 0, sizeof(Group));
	}
	else if(m_groups.size() == 1) {
		itr = m_groups.begin();
		group = *itr;
	}

	switch (surfaceIndex) {
		case EURO_FEM_NOSE_BRIDGE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			m_numberOfFaces = nFEFShortBridgeNose;
			group->faces = (Face *)(&facesEFShortBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_DEEP:
			m_vertices = (vec3 *)(&vertEFDeepBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFDeepBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFDeepBridgeNose[0]);
			m_numberOfVertices = nVEFDeepBridgeNose;
			m_textureCoordsCount = nTEFDeepBridgeNose;
			m_numberOfFaces = nFEFDeepBridgeNose;
			group->faces = (Face *)(&facesEFDeepBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_LONG:
			m_vertices = (vec3 *)(&vertEFLongBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFLongBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFLongBridgeNose[0]);
			m_numberOfVertices = nVEFLongBridgeNose;
			m_textureCoordsCount = nTEFLongBridgeNose;	
			m_numberOfFaces = nFEFLongBridgeNose;
			group->faces = (Face *)(&facesEFLongBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_SHALLOW:
			m_vertices = (vec3 *)(&vertEFShallowBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShallowBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShallowBridgeNose[0]);
			m_numberOfVertices = nVEFShallowBridgeNose;
			m_textureCoordsCount = nTEFShallowBridgeNose;	
			m_numberOfFaces = nFEFShallowBridgeNose;
			group->faces = (Face *)(&facesEFShallowBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_DOWN:
			m_vertices = (vec3 *)(&vertEFDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFDownNose[0]);
			m_textureCoords = (float *)(&vertTEFDownNose[0]);
			m_numberOfVertices = nVEFDownNose;
			m_textureCoordsCount = nTEFDownNose;	
			m_numberOfFaces = nFEFDownNose;
			group->faces = (Face *)(&facesEFDownNose[0]);
			break;
		case EURO_FEM_NOSE_FLAT:
			m_vertices = (vec3 *)(&vertEFFlatNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFFlatNose[0]);
			m_textureCoords = (float *)(&vertTEFFlatNose[0]);
			m_numberOfVertices = nVEFFlatNose;
			m_textureCoordsCount = nTEFFlatNose;
			m_numberOfFaces = nFEFFlatNose;
			group->faces = (Face *)(&facesEFFlatNose[0]);
			break;
		case EURO_FEM_NOSE_LONG:
			m_vertices = (vec3 *)(&vertEFLongNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFLongNose[0]);
			m_textureCoords = (float *)(&vertTEFLongNose[0]);
			m_numberOfVertices = nVEFLongNose;
			m_textureCoordsCount = nTEFLongNose;
			m_numberOfFaces = nFEFLongNose;
			group->faces = (Face *)(&facesEFLongNose[0]);
			break;
		case EURO_FEM_NOSE_POINTED:
			m_vertices = (vec3 *)(&vertEFPointedNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFPointedNose[0]);
			m_textureCoords = (float *)(&vertTEFPointedNose[0]);
			m_numberOfVertices = nVEFPointedNose;
			m_textureCoordsCount = nTEFPointedNose;
			m_numberOfFaces = nFEFPointedNose;
			group->faces = (Face *)(&facesEFPointedNose[0]);
			break;
		case EURO_FEM_NOSE_REGION_CONCAVE:
			m_vertices = (vec3 *)(&vertEFConcaveRegionNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFConcaveRegionNose[0]);
			m_textureCoords = (float *)(&vertTEFConcaveRegionNose[0]);
			m_numberOfVertices = nVEFConcaveRegionNose;
			m_textureCoordsCount = nTEFConcaveRegionNose;
			m_numberOfFaces = nFEFConcaveRegionNose;
			group->faces = (Face *)(&facesEFConcaveRegionNose[0]);
			break;
		case EURO_FEM_NOSE_REGION_CONVEX:
			m_vertices = (vec3 *)(&vertEFConvexRegionNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFConvexRegionNose[0]);
			m_textureCoords = (float *)(&vertTEFConvexRegionNose[0]);
			m_numberOfVertices = nVEFConvexRegionNose;
			m_textureCoordsCount = nTEFConvexRegionNose;
			m_numberOfFaces = nFEFConvexRegionNose;
			group->faces = (Face *)(&facesEFConvexRegionNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_DEEP:
			m_vertices = (vec3 *)(&vertEFSellionDeepNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionDeepNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionDeepNose[0]);
			m_numberOfVertices = nVEFSellionDeepNose;
			m_textureCoordsCount = nTEFSellionDeepNose;
			m_numberOfFaces = nFEFSellionDeepNose;
			group->faces = (Face *)(&facesEFSellionDeepNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_DOWN:
			m_vertices = (vec3 *)(&vertEFSellionDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionDownNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionDownNose[0]);
			m_numberOfVertices = nVEFSellionDownNose;
			m_textureCoordsCount = nTEFSellionDownNose;
			m_numberOfFaces = nFEFSellionDownNose;
			group->faces = (Face *)(&facesEFSellionDownNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_SHALLOW:
			m_vertices = (vec3 *)(&vertEFSellionShallowNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionShallowNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionShallowNose[0]);
			m_numberOfVertices = nVEFSellionShallowNose;
			m_textureCoordsCount = nTEFSellionShallowNose;
			m_numberOfFaces = nFEFSellionShallowNose;
			group->faces = (Face *)(&facesEFSellionShallowNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_THIN:
			m_vertices = (vec3 *)(&vertEFSellionThinNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionThinNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionThinNose[0]);
			m_numberOfVertices = nVEFSellionThinNose;
			m_textureCoordsCount = nTEFSellionThinNose;
			m_numberOfFaces = nFEFSellionThinNose;
			group->faces = (Face *)(&facesEFSellionThinNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_UP:
			m_vertices = (vec3 *)(&vertEFSellionUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionUpNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionUpNose[0]);
			m_numberOfVertices = nVEFSellionUpNose;
			m_textureCoordsCount = nTEFSellionUpNose;
			m_numberOfFaces = nFEFSellionUpNose;
			group->faces = (Face *)(&facesEFSellionUpNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_WIDE:
			m_vertices = (vec3 *)(&vertEFSellionWideNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionWideNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionWideNose[0]);
			m_numberOfVertices = nVEFSellionWideNose;
			m_textureCoordsCount = nTEFSellionWideNose;
			m_numberOfFaces = nFEFSellionWideNose;
			group->faces = (Face *)(&facesEFSellionWideNose[0]);
			break;
		case EURO_FEM_NOSE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortNose[0]);
			m_textureCoords = (float *)(&vertTEFShortNose[0]);
			m_numberOfVertices = nVEFShortNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			m_numberOfFaces = nFEFShortNose;
			group->faces = (Face *)(&facesEFShortNose[0]);
			break;
		case EURO_FEM_NOSE_TILT_DOWN:
			m_vertices = (vec3 *)(&vertEFTiltDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFTiltDownNose[0]);
			m_textureCoords = (float *)(&vertTEFTiltDownNose[0]);
			m_numberOfVertices = nVEFTiltDownNose;
			m_textureCoordsCount = nTEFTiltDownNose;
			m_numberOfFaces = nFEFTiltDownNose;
			group->faces = (Face *)(&facesEFTiltDownNose[0]);
			break;
		case EURO_FEM_NOSE_TILT_UP:
			m_vertices = (vec3 *)(&vertEFTiltUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFTiltUpNose[0]);
			m_textureCoords = (float *)(&vertTEFTiltUpNose[0]);
			m_numberOfVertices = nVEFTiltUpNose;
			m_textureCoordsCount = nTEFTiltUpNose;
			m_numberOfFaces = nFEFTiltUpNose;
			group->faces = (Face *)(&facesEFTiltUpNose[0]);
			break;
		case EURO_FEM_NOSE_UP:
			m_vertices = (vec3 *)(&vertEFNoseUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFNoseUpNose[0]);
			m_textureCoords = (float *)(&vertTEFNoseUpNose[0]);
			m_numberOfVertices = nVEFNoseUpNose;
			m_textureCoordsCount = nTEFNoseUpNose;
			m_numberOfFaces = nFEFNoseUpNose;
			group->faces = (Face *)(&facesEFNoseUpNose[0]);
			break;
		case EURO_FEM_NOSTRIL_TILT_DOWN:
			m_vertices = (vec3 *)(&vertEFNostrilTiltDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFNostrilTiltDownNose[0]);
			m_textureCoords = (float *)(&vertTEFNostrilTiltDownNose[0]);
			m_numberOfVertices = nVEFNostrilTiltDownNose;
			m_textureCoordsCount = nTEFNostrilTiltDownNose;
			m_numberOfFaces = nFEFNostrilTiltDownNose;
			group->faces = (Face *)(&facesEFNostrilTiltDownNose[0]);
			break;
#if 0
		case EURO_FEM_NOSE_NOSTRIL_TILT_UP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_FEM_NOSTRILS_LARGE:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_FEM_NOSTRILS_SMALL:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_FEM_NOSTRILS_THIN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_FEM_NOSTRILS_WIDE:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_AVG:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_BRIDGE_DEEP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_BRIDGE_LONG:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_BRIDGE_SHALLOW:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
			//case EURO_NOSE_BRIDGE_SHORT1:
			//	name = string("/euro-nose-bridge-short1.obj");
			//	break;
		case EURO_NOSE_BRIDGE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_DOWN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_FEM_AVG:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_FLAT:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_LONG:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
			//case EURO_NOSE_MALE_AVG:
			//	name = string("/euro-nose-male-avg.obj");
			//	break;
			//case EURO_NOSE_NOSTRIL_DOWN:
			//	name = string("/euro-nose-nostril-down.obj");
			//	break;
		case EURO_NOSE_NOSTRIL_THIN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_NOSTRIL_UP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_NOSTRIL_WIDE:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_POINTED:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_REGION_CONCAVE:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_REGION_CONVEX:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_DEEP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_DOWN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_SHALLOW:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_THIN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_UP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SELLION_WIDE:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_TILT_DOWN:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_TILT_UP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
		case EURO_NOSE_UP:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			
			break;
#endif
		default:
			break;
	}
#if 1	
	group->numberOfFaces = m_numberOfFaces;
	group->material = NULL;
	if(m_groups.size() == 0) {
		m_groups.push_back(group);
	}
#endif
#endif
}


void ObjSurface::InitVertices(int surfaceIndex)
{
	vector<Group *> ::iterator itr;
	Group *group;
	
	if(m_groups.size() == 0 ) {
		m_groups.clear();
		group = (Group *)malloc(sizeof(Group));
		memset(group, 0, sizeof(Group));
	}
	else if(m_groups.size() == 1) {
		itr = m_groups.begin();
		group = *itr;
	}
	
	switch (surfaceIndex) {
		case EURO_FEM_NOSE_BRIDGE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShortBridgeNose[0]);
			m_numberOfVertices = nVEFShortBridgeNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			m_numberOfFaces = nFEFShortBridgeNose;
			group->faces = (Face *)(&facesEFShortBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_DEEP:
			m_vertices = (vec3 *)(&vertEFDeepBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFDeepBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFDeepBridgeNose[0]);
			m_numberOfVertices = nVEFDeepBridgeNose;
			m_textureCoordsCount = nTEFDeepBridgeNose;
			m_numberOfFaces = nFEFDeepBridgeNose;
			group->faces = (Face *)(&facesEFDeepBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_LONG:
			m_vertices = (vec3 *)(&vertEFLongBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFLongBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFLongBridgeNose[0]);
			m_numberOfVertices = nVEFLongBridgeNose;
			m_textureCoordsCount = nTEFLongBridgeNose;	
			m_numberOfFaces = nFEFLongBridgeNose;
			group->faces = (Face *)(&facesEFLongBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_BRIDGE_SHALLOW:
			m_vertices = (vec3 *)(&vertEFShallowBridgeNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShallowBridgeNose[0]);
			m_textureCoords = (float *)(&vertTEFShallowBridgeNose[0]);
			m_numberOfVertices = nVEFShallowBridgeNose;
			m_textureCoordsCount = nTEFShallowBridgeNose;	
			m_numberOfFaces = nFEFShallowBridgeNose;
			group->faces = (Face *)(&facesEFShallowBridgeNose[0]);
			break;
		case EURO_FEM_NOSE_DOWN:
			m_vertices = (vec3 *)(&vertEFDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFDownNose[0]);
			m_textureCoords = (float *)(&vertTEFDownNose[0]);
			m_numberOfVertices = nVEFDownNose;
			m_textureCoordsCount = nTEFDownNose;	
			m_numberOfFaces = nFEFDownNose;
			group->faces = (Face *)(&facesEFDownNose[0]);
			break;
		case EURO_FEM_NOSE_FLAT:
			m_vertices = (vec3 *)(&vertEFFlatNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFFlatNose[0]);
			m_textureCoords = (float *)(&vertTEFFlatNose[0]);
			m_numberOfVertices = nVEFFlatNose;
			m_textureCoordsCount = nTEFFlatNose;
			m_numberOfFaces = nFEFFlatNose;
			group->faces = (Face *)(&facesEFFlatNose[0]);
			break;
		case EURO_FEM_NOSE_LONG:
			m_vertices = (vec3 *)(&vertEFLongNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFLongNose[0]);
			m_textureCoords = (float *)(&vertTEFLongNose[0]);
			m_numberOfVertices = nVEFLongNose;
			m_textureCoordsCount = nTEFLongNose;
			m_numberOfFaces = nFEFLongNose;
			group->faces = (Face *)(&facesEFLongNose[0]);
			break;
		case EURO_FEM_NOSE_POINTED:
			m_vertices = (vec3 *)(&vertEFPointedNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFPointedNose[0]);
			m_textureCoords = (float *)(&vertTEFPointedNose[0]);
			m_numberOfVertices = nVEFPointedNose;
			m_textureCoordsCount = nTEFPointedNose;
			m_numberOfFaces = nFEFPointedNose;
			group->faces = (Face *)(&facesEFPointedNose[0]);
			break;
		case EURO_FEM_NOSE_REGION_CONCAVE:
			m_vertices = (vec3 *)(&vertEFConcaveRegionNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFConcaveRegionNose[0]);
			m_textureCoords = (float *)(&vertTEFConcaveRegionNose[0]);
			m_numberOfVertices = nVEFConcaveRegionNose;
			m_textureCoordsCount = nTEFConcaveRegionNose;
			m_numberOfFaces = nFEFConcaveRegionNose;
			group->faces = (Face *)(&facesEFConcaveRegionNose[0]);
			break;
		case EURO_FEM_NOSE_REGION_CONVEX:
			m_vertices = (vec3 *)(&vertEFConvexRegionNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFConvexRegionNose[0]);
			m_textureCoords = (float *)(&vertTEFConvexRegionNose[0]);
			m_numberOfVertices = nVEFConvexRegionNose;
			m_textureCoordsCount = nTEFConvexRegionNose;
			m_numberOfFaces = nFEFConvexRegionNose;
			group->faces = (Face *)(&facesEFConvexRegionNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_DEEP:
			m_vertices = (vec3 *)(&vertEFSellionDeepNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionDeepNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionDeepNose[0]);
			m_numberOfVertices = nVEFSellionDeepNose;
			m_textureCoordsCount = nTEFSellionDeepNose;
			m_numberOfFaces = nFEFSellionDeepNose;
			group->faces = (Face *)(&facesEFSellionDeepNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_DOWN:
			m_vertices = (vec3 *)(&vertEFSellionDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionDownNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionDownNose[0]);
			m_numberOfVertices = nVEFSellionDownNose;
			m_textureCoordsCount = nTEFSellionDownNose;
			m_numberOfFaces = nFEFSellionDownNose;
			group->faces = (Face *)(&facesEFSellionDownNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_SHALLOW:
			m_vertices = (vec3 *)(&vertEFSellionShallowNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionShallowNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionShallowNose[0]);
			m_numberOfVertices = nVEFSellionShallowNose;
			m_textureCoordsCount = nTEFSellionShallowNose;
			m_numberOfFaces = nFEFSellionShallowNose;
			group->faces = (Face *)(&facesEFSellionShallowNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_THIN:
			m_vertices = (vec3 *)(&vertEFSellionThinNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionThinNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionThinNose[0]);
			m_numberOfVertices = nVEFSellionThinNose;
			m_textureCoordsCount = nTEFSellionThinNose;
			m_numberOfFaces = nFEFSellionThinNose;
			group->faces = (Face *)(&facesEFSellionThinNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_UP:
			m_vertices = (vec3 *)(&vertEFSellionUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionUpNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionUpNose[0]);
			m_numberOfVertices = nVEFSellionUpNose;
			m_textureCoordsCount = nTEFSellionUpNose;
			m_numberOfFaces = nFEFSellionUpNose;
			group->faces = (Face *)(&facesEFSellionUpNose[0]);
			break;
		case EURO_FEM_NOSE_SELLION_WIDE:
			m_vertices = (vec3 *)(&vertEFSellionWideNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFSellionWideNose[0]);
			m_textureCoords = (float *)(&vertTEFSellionWideNose[0]);
			m_numberOfVertices = nVEFSellionWideNose;
			m_textureCoordsCount = nTEFSellionWideNose;
			m_numberOfFaces = nFEFSellionWideNose;
			group->faces = (Face *)(&facesEFSellionWideNose[0]);
			break;
		case EURO_FEM_NOSE_SHORT:
			m_vertices = (vec3 *)(&vertEFShortNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFShortNose[0]);
			m_textureCoords = (float *)(&vertTEFShortNose[0]);
			m_numberOfVertices = nVEFShortNose;
			m_textureCoordsCount = nTEFShortBridgeNose;
			m_numberOfFaces = nFEFShortNose;
			group->faces = (Face *)(&facesEFShortNose[0]);
			break;
		case EURO_FEM_NOSE_TILT_DOWN:
			m_vertices = (vec3 *)(&vertEFTiltDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFTiltDownNose[0]);
			m_textureCoords = (float *)(&vertTEFTiltDownNose[0]);
			m_numberOfVertices = nVEFTiltDownNose;
			m_textureCoordsCount = nTEFTiltDownNose;
			m_numberOfFaces = nFEFTiltDownNose;
			group->faces = (Face *)(&facesEFTiltDownNose[0]);
			break;
		case EURO_FEM_NOSE_TILT_UP:
			m_vertices = (vec3 *)(&vertEFTiltUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFTiltUpNose[0]);
			m_textureCoords = (float *)(&vertTEFTiltUpNose[0]);
			m_numberOfVertices = nVEFTiltUpNose;
			m_textureCoordsCount = nTEFTiltUpNose;
			m_numberOfFaces = nFEFTiltUpNose;
			group->faces = (Face *)(&facesEFTiltUpNose[0]);
			break;
#if 1
		case EURO_FEM_NOSE_UP:
			m_vertices = (vec3 *)(&vertEFNoseUpNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFNoseUpNose[0]);
			m_textureCoords = (float *)(&vertTEFNoseUpNose[0]);
			m_numberOfVertices = nVEFNoseUpNose;
			m_textureCoordsCount = nTEFNoseUpNose;
			m_numberOfFaces = nFEFNoseUpNose;
			group->faces = (Face *)(&facesEFNoseUpNose[0]);
			break;
			
		case EURO_FEM_NOSTRIL_TILT_DOWN:
			m_vertices = (vec3 *)(&vertEFNostrilTiltDownNose[0]);
			m_vertexNormals = (vec3 *)(&vertNEFNostrilTiltDownNose[0]);
			m_textureCoords = (float *)(&vertTEFNostrilTiltDownNose[0]);
			m_numberOfVertices = nVEFNostrilTiltDownNose;
			m_textureCoordsCount = nTEFNostrilTiltDownNose;
			m_numberOfFaces = nFEFNostrilTiltDownNose;
			group->faces = (Face *)(&facesEFNostrilTiltDownNose[0]);
			break;
#endif
		default:
			break;
	}
	
	group->numberOfFaces = m_numberOfFaces;
	group->material = NULL;
	if(m_groups.size() == 0) {
		m_groups.push_back(group);
	}
	
}

#else

ObjSurface::ObjSurface() :
m_name("none"),
m_faceCount(0),
m_vertexCount(0),
m_groupCount(0),
m_textureCoordsCount(0),
m_numberOfVertices(0),
m_numberOfFaces(0),
m_textureCoords(NULL),
m_valuesPerCoord(0),
m_vertices(NULL),
m_vertexNormals(NULL),
m_allTextureCoords(NULL),
m_rootNode(NULL),
m_eolChar('\r'),
m_surfaceType(EYE_GEN_100)

{

}


void ObjSurface::GetGroupAndVerticesOLD(vector<Group *> *g, vec3 **vertices, unsigned int *nVertices, unsigned int *nFaces,
							vec3 **sNormals, vec3 **vNormals, float **texCoords, unsigned int *vPerCoord, unsigned int *)
{
	return;
}

void ObjSurface::GetGroupAndVertices(int index, vector<Group *> *g, vec3 **vert, unsigned int *nVertices, unsigned int *nFaces,
									 vec3 **sNormals, vec3 **vNormals, float **texCoords, 
									 unsigned int *vPerCoord, unsigned int *texCoordsCount, int sType)

{
	m_surfaceType = sType;
	InitVertices(index);
	*vert = m_vertices;
	*nVertices = m_numberOfVertices;
	*vNormals = m_vertexNormals;
	*nFaces = m_numberOfFaces;
	
	*texCoords = m_textureCoords;
	*vPerCoord = m_valuesPerCoord;
	*texCoordsCount = m_textureCoordsCount;
	
	vector<Group *>::iterator itr;
	for(itr=m_groups[index].begin(); itr != m_groups[index].end(); itr++)
	{
		(*g).push_back(*itr);
		
		
	}
	
}



void ObjSurface::InitVertices(int surfaceIndex)
{
	vector<Group *> ::iterator itr;
	Group *group;
	
	if(m_groups[surfaceIndex].size() == 0 ) {
		m_groups[surfaceIndex].clear();
		group = (Group *)malloc(sizeof(Group));
		memset(group, 0, sizeof(Group));
	}
	else if(m_groups[surfaceIndex].size() == 1) {
		itr = m_groups[surfaceIndex].begin();
		group = *itr;
	}
	switch(m_surfaceType) {
		case EYE_110:
		case EYE_128_BLUE7:
		case EYE_128_BLUE8:
		case EYE_128_PURPLE1:
		case EYE_128_PURPLE2:
		case EYE_128_DEEPBLUE:
		case EYE_128_BROWN1:
		case EYE_128_BROWN2:
		case EYE_128_BROWN3:
		case EYE_128_RED1:
		case EYE_128_RED2:
		case EYE_111:
		case EYE_112:
		case EYE_113:
		case EYE_114:
		case EYE_115:
		case EYE_116:
		case EYE_117:
		case EYE_118:
		case EYE_119:
		case EYE_120:
		case EYE_121:
		case EYE_122:
		case EYE_123:
		case EYE_124:
		case EYE_125:
		case EYE_126:
		case EYE_127:
		case EYE_128:
		case EYE_129:
		case EYE_130:
		case EYE_131:
		case EYE_132:
		case EYE_133:
		case EYE_134:
		case EYE_135:
		case EYE_136:
		case EYE_137:
		case EYE_138:
		case EYE_139:
		case EYE_140:
		case EYE_141:
		case EYE_GEN_100:
		case EYE_41:
		case EYE_42:
		case EYE_43:
		case EYE_44:
		case EYE_45:
		case EYE_46:
		case EYE_47:
		case EYE_48:
		case EYE_49:
		case EYE_50:
		case EYE_51:
			m_vertices = (vec3 *)(&vertEye111[0]);
			m_vertexNormals = (vec3 *)(&vertNEye111[0]);
			m_textureCoords = (float *)(&vertTEye111[0]);
			m_numberOfVertices = nVEye111;
			m_textureCoordsCount = nTEye111;
			m_numberOfFaces = nFEye111;
			group->faces = (Face *)(&facesEye111[0]);
			break;		
#if 0
		case EYE_GEN_100:
			m_vertices = (vec3 *)(&vertEye100[0]);
			m_vertexNormals = (vec3 *)(&vertNEye100[0]);
			m_textureCoords = (float *)(&vertTEye100[0]);
			m_numberOfVertices = nVEye100;
			m_textureCoordsCount = nTEye100;
			m_numberOfFaces = nFEye100;
			group->faces = (Face *)(&facesEye100[0]);
			break;

		case EYE_256_BLUE1:
		case EYE_256_BLUE2:
		case EYE_256_BLUE3:
		case EYE_256_BLUE4:
		case EYE_256_BLUE5:
		case EYE_256_BLUE6:
		case EYE_256_BLUE7:
		case EYE_256_BLUE8:
		case EYE_256_DEEPBLUE:
		case EYE_256_BROWN1:
		case EYE_256_BROWN2:
		case EYE_256_BROWN3:
		case EYE_256_RED1:
		case EYE_256_RED2:
		case EYE_256_GREEN1:
		case EYE_256_GREEN2:
		case EYE_256_GREEN3:
		case EYE_256_GREEN4:
		case EYE_256_GREEN5:
		case EYE_256_GREEN6:
		case EYE_256_GREEN7:
		case EYE_256_GREEN8:
		case EYE_256_GREEN9:
		case EYE_256_PURPLE1:
		case EYE_256_PURPLE2:
		case EYE_256_PURPLE3:
			m_vertices = (vec3 *)(&vertEye256[0]);
			m_vertexNormals = (vec3 *)(&vertNEye256[0]);
			m_textureCoords = (float *)(&vertTEye256[0]);
			m_numberOfVertices = nVEye256;
			m_textureCoordsCount = nTEye256;
			m_numberOfFaces = nFEye256;
			group->faces = (Face *)(&facesEye256[0]);
			break;
#endif
		default:
			m_vertices = (vec3 *)(&vertEye111[0]);
			m_vertexNormals = (vec3 *)(&vertNEye111[0]);
			m_textureCoords = (float *)(&vertTEye111[0]);
			m_numberOfVertices = nVEye111;
			m_textureCoordsCount = nTEye111;
			m_numberOfFaces = nFEye111;
			group->faces = (Face *)(&facesEye111[0]);
			break;		
	}
	
	group->numberOfFaces = m_numberOfFaces;
	group->material = NULL;
	if(m_groups[surfaceIndex].size() == 0) {
		m_groups[surfaceIndex].push_back(group);
	}
}

#endif
