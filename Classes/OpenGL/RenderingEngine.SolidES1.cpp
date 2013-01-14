#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "Interfaces.hpp"
#include "Matrix.hpp"
#include <iostream>
#import <QuartzCore/QuartzCore.h>
#import "ConstantsAndMacros.h"
#include "NTIMalloc.h"

using namespace std;
namespace SolidES1 {

struct Drawable {
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    int IndexCount;
};

struct DrawableTexture {
	vector<GLuint> VertexBuffer;
	GLuint IndexBuffer;
	int IndexCount;
};

// Renderer capabilities that affect this application
typedef struct {
	GLboolean extension[NUM_EXTENSIONS];
	GLint     maxTextureSize;
} RendererInfo;
	
class RenderingEngine : public IRenderingEngine {
public:
    RenderingEngine();
	RenderingEngine(IResourceManager* resourceManager);
    void Initialize(const vector<ISurface*>& surfaces);
	void InitializeBoundingSphere(ISurface *s, int index);
	void InitializeMY(const vector<ISurface*>& surfaces);
    void Render(const vector<Visual>& visuals) ;
//	void RenderMY(const vector<Visual>& visuals, int) ;
	void RenderMY(const Visual * visuals) ;
	bool PresentBuffer() {return m_presentBuffer;}
	void SetNoseTextureData(void *dat, int w, int h);
	void SetNoseTextureData1(GLuint tex) ;
	void SetMatProperty(int p, float value);
	void DeallocCurrentObjectOLD();
	void DeallocCurrentObject();
	void SetAlphaCorrection(int val) {m_alphaCorrection = val;};
	void ToggleLight(int val);
	void InitRenderingArrays(int surfaceIndex);
	void EnableLargeAngles(int val) { m_supportWideAngleRotations = val;};
	void LoadTexture(GLuint tex); 
	void ClearBuffer();
	void SetSurfaceTypeIndex(int n) {m_surfaceType = n;};
private:
	void InitializeBoundingSphereOLD(ISurface *s, int index);
	void SetTexture();
	void SetPvrTexture(const string& filename) const;
	void SetPngTexture(const string& filename) const;
	bool m_presentBuffer;
	//vector<vec4> m_surfaceBoundingSphere;
	vec4 m_surfaceBoundingSphere[SurfaceCount];
    vector<Drawable> m_drawables;
	vector<DrawableTexture> m_drawablesT;
	vector<Group *>		m_groups[SurfaceCount];
	vec3 *m_vertices[SurfaceCount];
	vec3 *m_vertexNormals[SurfaceCount];
	vec3 *m_surfaceNormals[SurfaceCount];
	unsigned int m_numberOfVertices[SurfaceCount];
	float *m_textureCoords[SurfaceCount];
	unsigned int m_valuesPerCoord[SurfaceCount];
	unsigned int m_textureCoordsCount[SurfaceCount];
	int m_currentSurface;
	GLuint m_frameBuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
	GLuint m_depthStencil;
	GLint m_backingWidth;
	GLint m_backingHeight;
    mat4 m_translation;
	void *imageData;
	int imageWidth;
	int imageHeight;
	vec4 m_ambient;
	vec4 m_diffuse;
	vec4 m_specular;
	float m_shininess;
	int m_supportWideAngleRotations;
	int m_matPropertyEnabled;
	int m_varyAlpha;
	int m_alphaCorrection;
	int m_lightState[MAX_LIGHT];
	int m_surfaceType;
	ivec2 m_currSize[SurfaceCount];
	ivec2 m_lowerLeft[SurfaceCount];
	RendererInfo renderer;
	IResourceManager* m_resourceManager;
};
    
IRenderingEngine* CreateRenderingEngine()
{
    return new RenderingEngine();
}

IRenderingEngine* CreateRenderingEngine(IResourceManager* resourceManager)
{
	return new RenderingEngine(resourceManager);
}
	
RenderingEngine::RenderingEngine()
{
    glGenRenderbuffersOES(1, &m_colorRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
}

RenderingEngine::RenderingEngine(IResourceManager* resourceManager)
{
	m_resourceManager = resourceManager;
	glGenFramebuffersOES(1, &m_frameBuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_frameBuffer);
	glGenRenderbuffersOES(1, &m_colorRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
	
	m_ambient		= vec4(0.2, 0.2, 0.2, 1.0);
	m_diffuse		= vec4(1.0, 1.0, 1.0, 1.0);
	m_specular		= vec4(1.0, 1.0, 1.0, 1.0);
	m_shininess		= 0.0;
	
	m_matPropertyEnabled = FALSE;
	m_varyAlpha = TRUE;
	m_currentSurface = 0;
	m_alphaCorrection = 0;
	for(int i=0; i < MAX_LIGHT; i++) {
		m_lightState[i] = 0;
	}
	m_supportWideAngleRotations = 1;
	m_surfaceType = 0;
	memset((void *)(&renderer), 0, sizeof(RendererInfo));
}
	
void RenderingEngine::Initialize(const vector<ISurface*>& surfaces)
{
#ifdef USE_ORIG_CODE
    vector<ISurface*>::const_iterator surface;
    for (surface = surfaces.begin(); surface != surfaces.end(); ++surface) {
        
        // Create the VBO for the vertices.
        vector<float> vertices;
        (*surface)->GenerateVertices(vertices, VertexFlagsNormals);
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices[0]),
					 &vertices[0],
                     GL_STATIC_DRAW);
        
		vec4 sphere = (*surface)->GetBoundingSphere();
		//m_surfaceBoundingSphere.push_back(sphere);
		m_surfaceBoundingSphere = sphere;
		
        // Create a new VBO for the indices if needed.
        int indexCount = (*surface)->GetTriangleIndexCount();
        GLuint indexBuffer;
        if (!m_drawables.empty() && indexCount == m_drawables[0].IndexCount) {
            indexBuffer = m_drawables[0].IndexBuffer;
        } else {
            vector<GLushort> indices(indexCount);
            (*surface)->GenerateTriangleIndices(indices);
            glGenBuffers(1, &indexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indexCount * sizeof(GLushort),
                         &indices[0],
                         GL_STATIC_DRAW);
        }
        
        Drawable drawable = { vertexBuffer, indexBuffer, indexCount};
        m_drawables.push_back(drawable);
    }
#endif
    // Extract width and height from the color buffer.
    int width, height;
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                    GL_RENDERBUFFER_WIDTH_OES, &width);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                    GL_RENDERBUFFER_HEIGHT_OES, &height);

    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffersOES(1, &m_depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_depthRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES,
                             width, height);

    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffersOES(1, &framebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
                                 GL_RENDERBUFFER_OES, m_colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
                                 GL_RENDERBUFFER_OES, m_depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);

    // Set up various GL state.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
#if 1
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); m_lightState[0] = 1;
	glEnable(GL_LIGHT1); m_lightState[1] = 1;
#endif
    glEnable(GL_DEPTH_TEST);

	GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) ;
	if(status != GL_FRAMEBUFFER_COMPLETE_OES) {
		cout << "failed to make complete framebuffer object %x" <<  glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) << endl;
		//return NO;
	}
	
    // Set up the material properties.
    //vec4 specular(0.5f, 0.5f, 0.5f, 1);
   // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.Pointer());
   // glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
	m_presentBuffer = false;
    m_translation = mat4::Translate(0, 0, -7);
}

void RenderingEngine::InitializeBoundingSphereOLD(ISurface *surface, int surfaceIndex)
{
#if 0
	vec3	*surfaceNormals;
	vec3	*vertexNormals;
	int n = surfaceIndex;
	
	unsigned int numberOfFaces=0;
	m_groups.clear();
#if 0
	Group *myg[5];
	surface->GetGroupAndVerticesOLD(myg, &m_vertices, &m_numberOfVertices, &numberOfFaces, 
									&surfaceNormals, &vertexNormals,
									&m_textureCoords, &m_valuesPerCoord, &m_textureCoordsCount);
#else
	surface->GetGroupAndVerticesOLD(&m_groups, &m_vertices, &m_numberOfVertices, &numberOfFaces, 
									&surfaceNormals, &vertexNormals,
									&m_textureCoords, &m_valuesPerCoord, &m_textureCoordsCount);
#endif
	m_vertexNormals = vertexNormals;
	m_surfaceNormals = surfaceNormals;
	//m_numberOfFaces = numberOfFaces;
	
#if 0
	cout << "unsigned int nF = " << numberOfFaces << endl ;
	cout << "----------------------------------BEGIN-----------------------------------------" << endl;
	cout << "unsigned int nV = " << m_numberOfVertices << endl ;
	cout << "unsigned int nT = " << m_textureCoordsCount << endl ;
	cout << "unsigned int nF = " << numberOfFaces << endl ;
	cout << "----------------------------------VERTICES-----------------------------------------" << endl;
	for(int v=0; v < (m_numberOfVertices + 10); v++)
	{
		cout << "vec3("  << m_vertices[v].x << "," << m_vertices[v].y << "," << m_vertices[v].z << ")," << endl;
		
	}
	cout << "-----------------------------------NORMALS----------------------------------------" << endl;
	for(int v=0; v < (m_numberOfVertices + 10); v++)
	{
		
		cout << "vec3("  << m_vertexNormals[v].x << "," << m_vertexNormals[v].y << "," << m_vertexNormals[v].z << "), " << endl;
	}
	cout << "-----------------------------------TEXTURE----------------------------------------" << endl;
	for(int v=0; v < ((m_textureCoordsCount + 10) * m_valuesPerCoord); v++)
	{
		cout <<  m_textureCoords[v] << ","  << endl;
	}
	cout << "-----------------------------------FACES----------------------------------------" << endl;
	Group *group = m_groups[0];
	for(int v=0; v < (numberOfFaces + 10); v++)
	{
		cout << "{"  << group->faces[v].v1 << "," << group->faces[v].v2 << "," << group->faces[v].v3 << "}, " << endl;
	}
	cout << "---------------------------------------END------------------------------------" << endl;
	
#endif	
	
	vec4 sphere = surface->GetBoundingSphere();
	m_surfaceBoundingSphere = sphere;
	m_currentSurface = n;
	
	vector<Group *> ::iterator itr;
	
	for(itr = m_groups.begin(); itr != m_groups.end(); itr++) 
	{
		
		Group *group = *itr;
		if(group) {
		
			group->oneTexture = TRUE;
			glGenTextures(1, &group->texture[0]);
			glBindTexture(GL_TEXTURE_2D, group->texture[0]);
			
		}
		 
	}
#endif		
	return;
	
}

void RenderingEngine::InitializeBoundingSphere(ISurface *surface, int surfaceIndex)
{
	
	float min_x = 1000.0f, min_y = 1000.0f, min_z = 1000.0f;
	float max_x = -1000.0f, max_y = -1000.0f, max_z = -1000.0f;
    float radius=0.0f;
	
	unsigned int numberOfFaces=0;
	for (int i = 0; i < SurfaceCount;i++) {
		m_currentSurface = i;
		m_groups[i].clear();
		m_currSize[i] =ivec2(32,32);
		m_lowerLeft[i] = ivec2(80, 280);
#if 0
		Group *myg;
		surface->GetGroupAndVertices(i, &myg, &m_vertices, &m_numberOfVertices, &numberOfFaces, 
								 &surfaceNormals, &vertexNormals,
								 &m_textureCoords, &m_valuesPerCoord, &m_textureCoordsCount);
#else
		surface->GetGroupAndVertices(i, &m_groups[i], &m_vertices[i], &m_numberOfVertices[i], &numberOfFaces, 
								 &m_surfaceNormals[i], &m_vertexNormals[i],
								 &m_textureCoords[i], &m_valuesPerCoord[i], &m_textureCoordsCount[i], m_surfaceType);
#endif
		m_valuesPerCoord[i] = 2;
		for(int v=0; v < m_numberOfVertices[i]; v++)
		{
			if(m_vertices[i][v].x < min_x) min_x = m_vertices[i][v].x;
			if(m_vertices[i][v].y < min_y) min_y = m_vertices[i][v].y;
			if(m_vertices[i][v].z < min_z) min_z = m_vertices[i][v].z;
		
			if(m_vertices[i][v].x > max_x) max_x = m_vertices[i][v].x;
			if(m_vertices[i][v].y > max_y) max_y = m_vertices[i][v].y;
			if(m_vertices[i][v].z > max_z) max_z = m_vertices[i][v].z;
		}
		vec3 center((min_x + max_x)/2.0f, (min_y + max_y)/2.0f, (min_z + max_z)/2.0f);
		radius = sqrt(
				  ((max_x - center.x) * (max_x - center.x)) +
				  ((max_y - center.y) * (max_y - center.y)) +
				  ((max_z - center.z) * (max_z - center.z))
				  );
		//radius *= 0.9; // HACK
		radius *= 1.25;
		m_surfaceBoundingSphere[i].x = center.x;
		m_surfaceBoundingSphere[i].y = center.y;
		m_surfaceBoundingSphere[i].z = center.z;
		m_surfaceBoundingSphere[i].w = radius;
	
		vector<Group *> ::iterator itr;
	
		for(itr = m_groups[i].begin(); itr != m_groups[i].end(); itr++) 
		{
		
			Group *group = *itr;
			if(group) {
			
				group->oneTexture = TRUE;
				glGenTextures(1, &group->texture[0]);
				//glBindTexture(GL_TEXTURE_2D, group->texture[0]);
			}
		}
	}
	
}

void RenderingEngine::InitializeMY(const vector<ISurface*>& surfaces)
{
	vector<ISurface*>::const_iterator surface;
	int n=0;
			   
#if 1
	
	const GLfloat			lightAmbient[] = {0.2, 0.2, 0.2, 1.0};
	const GLfloat			lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
	
	
	
	//const GLfloat			lightPosition[] = {0.0, 0.0, 15.0, 0.0}; 
	//const GLfloat			light1Position[] = {0.0, 240.0, 0.0, 0.0};
	//const GLfloat			light2Position[] = {160.0, 0.0, 0.0, 0.0};
	//const GLfloat			light3Position[] = {-160.0, 0.0, 0.0, 0.0};
	
	const GLfloat			lightShininess = 0.0;
	
	glEnable(GL_LIGHTING);
	const GLfloat			lightPosition[] = {5.0, 5.0, 15.0, 0.0};  // top right, from the front
	//glEnable(GL_LIGHT0); m_lightState[0]=1;
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 
	glLightfv(GL_LIGHT0, GL_SHININESS, &lightShininess);
#if 1	
	const GLfloat			light1Position[] = {-5.0, -5.0, 15.0, 0.0}; // bottom left from the front
	//glEnable(GL_LIGHT1); m_lightState[1]=1;
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position); 
	glLightfv(GL_LIGHT1, GL_SHININESS, &lightShininess);
#endif
#if 1
	const GLfloat			light2Position[] = {-5.0, 5.0, 15.0, 0.0}; // top left, from the font
	//glEnable(GL_LIGHT2); m_lightState[2]=1;
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Position); 
	glLightfv(GL_LIGHT2, GL_SHININESS, &lightShininess);
#endif
#if 1		
	const GLfloat			light3Position[] = {5.0, -5.0, 15.0, 0.0}; // top right, from the bottom
	//glEnable(GL_LIGHT3); m_lightState[3]=1;
	glLightfv(GL_LIGHT3, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT3, GL_POSITION, light3Position); 
	glLightfv(GL_LIGHT3, GL_SHININESS, &lightShininess);

#endif
#if 1
	const GLfloat			light4Position[] = {0.0, -5.0, 0.0, 0.0};  // from the bottom
	//glEnable(GL_LIGHT4); m_lightState[4]=1;
	glLightfv(GL_LIGHT4, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT4, GL_POSITION, light4Position); 
	glLightfv(GL_LIGHT4, GL_SHININESS, &lightShininess);
#endif	
#endif	
	
    glShadeModel(GL_SMOOTH); 
	surface = surfaces.begin();
	
   // for (surface = surfaces.begin(); surface != surfaces.end(); ++surface, n++) {
        
	InitializeBoundingSphere(*surface, n);	

	//}


	// Extract width and height from the color buffer.
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_WIDTH_OES, &m_backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_HEIGHT_OES, &m_backingHeight);
	
	// Create a depth buffer that has the same size as the color buffer.
	glGenRenderbuffersOES(1, &m_depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_depthRenderbuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES,
							 m_backingWidth, m_backingHeight);
			
	 glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, m_colorRenderbuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
								 GL_RENDERBUFFER_OES, m_depthRenderbuffer);
	//glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
	//						 GL_RENDERBUFFER_OES, m_depthStencil);
	//glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES,
	//						 GL_RENDERBUFFER_OES, m_depthStencil);
	 glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
	
	GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) ;
	if(status != GL_FRAMEBUFFER_COMPLETE_OES) {
		cout << "failed to make complete framebuffer object %x" <<  status << endl;
		//return NO;
	
	
	}

	// Query renderer capabilities that affect this app's rendering paths
	renderer.extension[APPLE_texture_2D_limited_npot] =
	(0 != strstr((char *)glGetString(GL_EXTENSIONS), "GL_APPLE_texture_2D_limited_npot"));
	renderer.extension[IMG_texture_format_BGRA8888] =
	(0 != strstr((char *)glGetString(GL_EXTENSIONS), "GL_IMG_texture_format_BGRA8888"));
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &renderer.maxTextureSize);
	
    // Set up various GL state.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
#if 1
    glEnable(GL_LIGHTING);
   // glEnable(GL_LIGHT0); m_lightState[0] = 1;
	//glEnable(GL_LIGHT1); m_lightState[1] = 1;
	glEnable(GL_LIGHT2); m_lightState[2] = 1;
	glEnable(GL_LIGHT3); m_lightState[3] = 1;
	//glEnable(GL_LIGHT4); m_lightState[4] = 1;
    glEnable(GL_DEPTH_TEST);
#endif
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	// Set up the material properties.
    //vec4 specular(0.5f, 0.5f, 0.5f, 1);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.Pointer());
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
	
	m_presentBuffer = false;
    m_translation = mat4::Translate(0, 0, -7);
	//m_supportWideAngleRotations = 0;
	
}


void RenderingEngine::Render(const vector<Visual>& visuals) 
{

	m_presentBuffer=false;
    //glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) {
        
		//vec4 sphere = m_surfaceBoundingSphere[visualIndex];
		vec4 sphere = m_surfaceBoundingSphere[visualIndex];
		vec2 screenPos = visual->ScreenPos;
		if((screenPos.x == -1000.0f) || (screenPos.y == 1000.0f)) continue;
		
        // Set the viewport transform.
        ivec2 size = visual->ViewportSize;
        ivec2 lowerLeft = visual->LowerLeft;
		
		lowerLeft.x = (int)screenPos.x;
		lowerLeft.y = (int)screenPos.y;
		//size.x=320 - lowerLeft.x;
		//size.y=480 - lowerLeft.y;
		glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
		
		GLfloat aspect = (float)(size.x)/(float)(size.y);
		
        // Set the light position.
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //vec4 lightPosition(0.25, 0.25, 1, 0);
        //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition.Pointer());
        
        // Set the model-view transform.
        mat4 rotation = visual->Orientation.ToMatrix();
		GLfloat h = 4.0f * size.y / size.x;
		GLfloat diam = 2 * sphere.w;
#if 0
		GLfloat left = sphere.x - diam;
		GLfloat right = sphere.x + diam;
		GLfloat bottom = sphere.y - diam;
		GLfloat top = sphere.y + diam;
#endif
		GLfloat zNearF = 0.001f;
		//GLfloat zNearO = 1.0f;
		//GLfloat zFarO = zNearO+diam;
		//GLfloat zFar = 1000.0f;
		GLfloat zFarF = zNearF+diam;
		GLfloat rMax = 0.0f;
		
        mat4 modelview = rotation * m_translation;
		mat4 m_translation1 = mat4::Translate(0, 0, -diam);
		modelview = rotation * m_translation1;
		
		
        glLoadMatrixf(modelview.Pointer());
		const GLfloat fieldOfView = 45.0;
		
		mat4 projection = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);
		h = 3.669f * ((float)size.y/(float)size.x);
		GLfloat zoom = 1.0f;
		
		double xmin, xmax, ymin, ymax;
		ymax = zNearF * tanf(fieldOfView * M_PI / 360.0);
		ymin = -ymax;
		xmin = ymin * aspect;
		xmax = ymax * aspect;
			
		rMax = (2 *diam) * sin((double)((fieldOfView * M_PI)/180)/2) *2.2;
		zoom = (float)(diam/2.0f)/rMax;

		projection = mat4::Frustum(xmin * zoom, xmax * zoom, ymin * zoom, ymax * zoom, zNearF, zFarF);
			
				
        //mat4 projection = mat4::Frustum(-2, 2, -h / 2, h / 2, 5.0f, 1000);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projection.Pointer());
					
		// Set the diffuse color.
        vec3 color = visual->Color * 0.75f;
        vec4 diffuse(color.x, color.y, color.z, 1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Pointer());

		// +++lal
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		m_presentBuffer = true;
        // Draw the surface.
        int stride = 2 * sizeof(vec3);
        const GLvoid* normalOffset = (const GLvoid*) sizeof(vec3);
        const Drawable& drawable = m_drawables[visualIndex];
        glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
        glVertexPointer(3, GL_FLOAT, stride, 0);
        glNormalPointer(GL_FLOAT, stride, normalOffset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.IndexBuffer);
        glDrawElements(GL_TRIANGLES, drawable.IndexCount, GL_UNSIGNED_SHORT, 0);
		
    }
	
}
	
#if 1

//void RenderingEngine::RenderMY(const vector<Visual>& visuals, int idx) 
void RenderingEngine::RenderMY(const Visual *visuals) 
{
	
	if(m_currentSurface == -1) return;
	m_presentBuffer=false;
	//glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//vector<Visual>::const_iterator visual = visuals.begin();
	//for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) 
	for(int visualIndex=0; visualIndex < SurfaceCount; visualIndex++)
	{
		
		//visual = visuals.begin();
		const Visual visual = visuals[visualIndex];
		//for(int si=0; si < idx; si++)
		//	visual++;
		vec4 sphere = m_surfaceBoundingSphere[visualIndex];
		//vec4 sphere = m_surfaceBoundingSphere;
		
		vec2 screenPos = visual.ScreenPos;
		//if((screenPos.x == -1000.0f) || (screenPos.y == -1000.0f)) continue;
		if((screenPos.x == -1000.0f) || (screenPos.y == -1000.0f)) return;
		vector<Group *> ::iterator itr;
		// Set the viewport transform.
		ivec2 size = visual.ViewportSize;
		ivec2 lowerLeft = visual.LowerLeft;
			
		lowerLeft.x = (int)screenPos.x;
		lowerLeft.y = (int)screenPos.y;
		
		// See if the object goes out of bounds
		int currX = lowerLeft.x + ((size.x)/2);
		int currY = lowerLeft.y + ((size.y)/2);
		int flag = 0;
		
		if((currX  > 300) || (currX  < 10)) flag = 1;
		if((currY > 450) || (currY < 60)) flag = 1;
		
		if(flag) {
			lowerLeft.x = m_lowerLeft[visualIndex].x;
			lowerLeft.y = m_lowerLeft[visualIndex].y;
			size.x = m_currSize[visualIndex].x;
			size.y = m_currSize[visualIndex].y;
		}
		else {
			m_lowerLeft[visualIndex].x = lowerLeft.x;
			m_lowerLeft[visualIndex].y = lowerLeft.y;
			m_currSize[visualIndex].x = size.x;
			m_currSize[visualIndex].y = size.y;
		}
		glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
		//glViewport(lowerLeft.x, lowerLeft.y, size.x*4, size.y*4);	
		GLfloat aspect = (float)(size.x)/(float)(size.y);
			
		// Set the light position.
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//vec4 lightPosition(0.25, 0.25, 1, 0);
		//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition.Pointer());
			
		// Set the model-view transform.
		mat4 rotation = visual.Orientation.ToMatrix();
		
		GLfloat diam = 2 * sphere.w;
#if 0
		GLfloat h = 4.0f * size.y / size.x;
		GLfloat left = sphere.x - diam;
		GLfloat right = sphere.x + diam;
		GLfloat bottom = sphere.y - diam;
		GLfloat top = sphere.y + diam;
#endif
		GLfloat zNearF = 0.001f;
		
		GLfloat zFarF = zNearF+diam;
		GLfloat rMax = 0.0f;
			
		
		m_translation = mat4::Translate(0, 0, -5);
		mat4 modelview = rotation * m_translation;
		//mat4 m_translation1 = mat4::Translate(0, 0, -diam);
		//modelview = rotation * m_translation1;
			
			
		glLoadMatrixf(modelview.Pointer());
		const GLfloat fieldOfView = 45.0;
		//const GLfloat fieldOfView = 89.0;
		//mat4 projection = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);
		//h = 3.669f * ((float)size.y/(float)size.x);
		GLfloat zoom = 1.0f;
			
		double xmin, xmax, ymin, ymax;
		ymax = zNearF * tanf(fieldOfView * M_PI / 360.0);
		ymin = -ymax;
		xmin = ymin * aspect;
		xmax = ymax * aspect;
			
		//rMax = (2 *diam) * sin((double)((fieldOfView * M_PI)/180)/2) *2.2;
		rMax = (2 *diam) * sin((double)((fieldOfView * M_PI)/180)/2) * 2.2;
		zoom = ((float)(diam/2.0f)/rMax) ;
		
		mat4 projection = mat4::Frustum(xmin * zoom, xmax * zoom, ymin * zoom, ymax * zoom, zNearF, zFarF);
			
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(projection.Pointer());
			
				
		// +++lal
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		m_presentBuffer = true;
		
		// Enable and load the vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &m_vertices[visualIndex][0]); 
		glNormalPointer(GL_FLOAT, 0, &m_vertexNormals[visualIndex][0]);
		
		
		
		// Loop through each group
		//vector<Group *> ::iterator itr;
#if 1
		//glMatrixMode(GL_TEXTURE);
		if (m_textureCoords[visualIndex] != NULL)
		{
			//glClientActiveTexture(GL_TEXTURE0+texUnit);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			
			glTexCoordPointer(m_valuesPerCoord[visualIndex], GL_FLOAT, 0, &m_textureCoords[visualIndex][0]);
		}
#endif
		
#if 0
		glEnable(GL_LIGHTING);
#endif
		//GLuint textureFrameBuffer;
		
		//for(int p=0; p < 2; p++) {
		for(itr = m_groups[visualIndex].begin(); itr != m_groups[visualIndex].end(); itr++)
		{
			
			Group *group = *itr;
			
			// clear buffers
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glClear(GL_COLOR_BUFFER_BIT );
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			
			if((group) && (group->oneTexture)) {
				
				SetNoseTextureData1(group->texture[0]);
			}

			
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
				cout << "Erroraaaaaa. glError: " << err << endl;
			
			if(m_matPropertyEnabled == TRUE) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const GLfloat *)&m_ambient);
				glColor4f(m_diffuse.x, m_diffuse.y, m_diffuse.z, m_diffuse.w);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  (const GLfloat *)&m_diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const GLfloat *)&m_specular);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);
			}
			

			for (int i=0; i < group->numberOfFaces; i++) {
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &(group->faces[i]));
			}
			
			GLenum err1 = glGetError();
			if (err1 != GL_NO_ERROR)
				cout << "Error11111111. glError: " << err1 << endl;
			 glDepthMask(GL_TRUE);
			
			//glDisable(GL_TEXTURE_2D);
		}
		//}
		if (m_textureCoords[visualIndex] != NULL)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		//glLoadIdentity();
			
	}
	//glPopMatrix();
}
#else
	
void RenderingEngine::RenderMYOLD(const vector<Visual>& visuals) 
{
	
	if(m_currentSurface == -1) return;
	m_presentBuffer=false;
	//glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	vector<Visual>::const_iterator visual = visuals.begin();
	//for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) 
	{
		
		//int visualIndex = m_currentSurface;
		//vec4 sphere = m_surfaceBoundingSphere[visualIndex];
		vec4 sphere = m_surfaceBoundingSphere;
		GLfloat diam = 2 * sphere.w;
		vec2 screenPos = visual->ScreenPos;
		//if((screenPos.x == -1000.0f) || (screenPos.y == -1000.0f)) continue;
		if((screenPos.x == -1000.0f) || (screenPos.y == -1000.0f)) return;
		vector<Group *> ::iterator itr;
		// Set the viewport transform.
		ivec2 size = visual->ViewportSize;
		ivec2 lowerLeft = visual->LowerLeft;
		
		lowerLeft.x = (int)screenPos.x;
		lowerLeft.y = (int)screenPos.y;
		//if(m_supportWideAngleRotations) {
		if(0) {
			int x = lowerLeft.x;
			int y = lowerLeft.y;
			int width = size.x*2;
			int height = size.y*2;
			
			x -= size.x;
			if(x < -(width/3)) x = -(width/3);
			else if((x + (width/2)) > 320) x = (int)(320.0f   - ((double)(width/2)) - diam);
			y -= size.y;
			if((y + (height/3)) < 53) y = (53 - (height/3));
			else if((y + (height/2)) > 480) y = (int)(480.0f - ((double)(height/2)) - diam);
			glViewport(x, y, width, height);
		}
		else {
			glViewport(lowerLeft.x, lowerLeft.y, size.x*4, size.y*4);
			//glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
		}
		GLfloat aspect = (float)(size.x)/(float)(size.y);
		
		// Set the light position.
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//vec4 lightPosition(0.25, 0.25, 1, 0);
		//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition.Pointer());
		
		// Set the model-view transform.
		mat4 rotation = visual->Orientation.ToMatrix();
		
		
#if 0
		GLfloat h = 4.0f * size.y / size.x;
		GLfloat left = sphere.x - diam;
		GLfloat right = sphere.x + diam;
		GLfloat bottom = sphere.y - diam;
		GLfloat top = sphere.y + diam;
#endif
		GLfloat zNearF = 0.001f;
		//GLfloat zNearO = 1.0f;
		//GLfloat zFarO = zNearO+diam;
		//GLfloat zFar = 1000.0f;
		GLfloat zFarF = zNearF+diam;
		GLfloat rMax = 0.0f;
		
		m_translation = mat4::Translate(0, 0, -5);
		mat4 modelview = rotation * m_translation;
		//mat4 m_translation1 = mat4::Translate(0, 0, -(diam/8.0f));
		//if(m_supportWideAngleRotations) 
		//	m_translation1 = mat4::Translate(0, 0, -(diam/0.75f));
	
		//mat4 modelview = rotation * m_translation1;
		
		glLoadMatrixf(modelview.Pointer());
		
		//mat4 projection = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);
		//h = 3.669f * ((float)size.y/(float)size.x);
		GLfloat zoom = 1.0f;
		const GLfloat fieldOfView = 45.0;
		//const GLfloat fieldOfView = 120.0;
		//if(m_supportWideAngleRotations) {
		//	140.0;
		//}
		double xmin, xmax, ymin, ymax;
		ymax = zNearF * tanf(fieldOfView * M_PI / 360.0);
		ymin = -ymax;
		xmin = ymin * aspect;
		xmax = ymax * aspect;
		
		//rMax = (2 *diam) * sin((double)((fieldOfView * M_PI)/180)/2) *2.2;
		rMax = (2 *diam) * sin((double)((fieldOfView * M_PI)/180)/2) * 2.2;
		zoom = ((float)(diam/2.0f)/rMax) ;
		//zoom /= 1.2f;
		mat4 projection = mat4::Frustum(xmin * zoom, xmax * zoom, ymin * zoom, ymax * zoom, zNearF, zFarF);
		
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(projection.Pointer());
		
		// +++lal
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		m_presentBuffer = true;
		
		// Enable and load the vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_vertices); 
		glNormalPointer(GL_FLOAT, 0, m_vertexNormals);
		
		if (m_textureCoords != NULL)
		{
			//glClientActiveTexture(GL_TEXTURE0+texUnit);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(m_valuesPerCoord, GL_FLOAT, 0, m_textureCoords);
		}
		glEnable(GL_LIGHTING);

				
		for(itr = m_groups.begin(); itr != m_groups.end(); itr++)
		//for(itr = m_groups[visualIndex].begin(); itr != m_groups[visualIndex].end(); itr++)
		{
			
			Group *group = *itr;
			
			if((group) && (group->oneTexture)) {
				
				SetNoseTextureData1(group->texture[0]);
				//LoadTexture(group->texture[0]);
			}
			
			// clear buffers
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT );
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
				cout << "Erroraaaaaa. glError: " << err << endl;
			
			if(m_matPropertyEnabled == TRUE) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const GLfloat *)&m_ambient);
				glColor4f(m_diffuse.x, m_diffuse.y, m_diffuse.z, m_diffuse.w);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  (const GLfloat *)&m_diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const GLfloat *)&m_specular);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);
			}
			
			
			for (int i=0; i < group->numberOfFaces; i++) {
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &(group->faces[i]));
			}
			
			GLenum err1 = glGetError();
			if (err1 != GL_NO_ERROR)
				cout << "Error11111111. glError: " << err1 << endl;
			glDepthMask(GL_TRUE);
			
			//glDisable(GL_TEXTURE_2D);
		}
		//}
		if (m_textureCoords != NULL)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		
	}
	//glPopMatrix();
}
#endif

void RenderingEngine::SetNoseTextureData(void *dat, int w, int h) 
{ 

	imageData =dat; 
	imageWidth =w; 
	imageHeight=h;
		
	return;

	
}

void RenderingEngine::SetTexture()
{
	switch(m_surfaceType) {
		
		case EYE_110:
			SetPvrTexture("eye110-128.pvr4");
			break;	
		case EYE_111:
			SetPvrTexture("eye111-128.pvr4");
			break;
		case EYE_112:
			SetPvrTexture("eye112-128.pvr4");
			break;
		case EYE_113:
			SetPvrTexture("eye113-128.pvr4");
			break;	
		case EYE_114:
			SetPvrTexture("eye114-128.pvr4");
			break;	
		case EYE_115:
			SetPvrTexture("eye115-128.pvr4");
			break;	
		case EYE_116:
			SetPvrTexture("eye116-128.pvr4");
			break;	
		case EYE_117:
			SetPvrTexture("eye117-128.pvr4");
			break;	
		case EYE_118:
			SetPvrTexture("eye118-128.pvr4");
			break;	
		case EYE_119:
			SetPvrTexture("eye119-128.pvr4");
			break;	
		case EYE_120:
			SetPvrTexture("eye120-128.pvr4");
			break;	
		case EYE_GEN_100:
			SetPvrTexture("eye100-256.pvr4");
			break;
		case EYE_121:
			SetPvrTexture("eye121-128.pvr4");
			break;
		case EYE_122:
			SetPvrTexture("eye122-128.pvr4");
			break;
		case EYE_123:
			SetPvrTexture("eye123-128.pvr4");
			break;
		case EYE_124:
			SetPvrTexture("eye124-128.pvr4");
			break;
		case EYE_125:
			SetPvrTexture("eye125-128.pvr4");
			break;
		case EYE_126:
			SetPvrTexture("eye126-128.pvr4");
			break;
		case EYE_127:
			SetPvrTexture("eye127-128.pvr4");
			break;
		case EYE_128:
			SetPvrTexture("eye128-128.pvr4");
			break;
		case EYE_129:
			SetPvrTexture("eye129-128.pvr4");
			break;
		case EYE_130:
			SetPvrTexture("eye130-128.pvr4");
			break;
		case EYE_131:
			SetPvrTexture("eye131-128.pvr4");
			break;
		case EYE_132:
			SetPvrTexture("eye132-128.pvr4");
			break;
		case EYE_133:
			SetPvrTexture("eye133-64.pvr4");
			break;
		case EYE_134:
			SetPvrTexture("eye134-64.pvr4");
			break;
		case EYE_135:
			SetPvrTexture("eye135-64.pvr4");
			break;
		case EYE_136:
			SetPvrTexture("eye136-128.pvr4");
			break;
		case EYE_137:
			SetPvrTexture("eye137-128.pvr4");
			break;
		case EYE_138:
			SetPvrTexture("eye138-128.pvr4");
			break;
		case EYE_139:
			SetPvrTexture("eye139-128.pvr4");
			break;
		case EYE_140:
			SetPvrTexture("eye140-64.pvr4");
			break;
		case EYE_141:
			SetPvrTexture("eye141-64.pvr4");
			break;
		case EYE_128_BLUE7:
			SetPvrTexture("blue7-128.pvr4");
			break;
		case EYE_128_BLUE8:
			SetPvrTexture("blue8-128.pvr4");
			break;
		case EYE_128_PURPLE1:
			SetPvrTexture("purple1-128.pvr4");
			break;
		case EYE_128_PURPLE2:
			SetPvrTexture("purple2-128.pvr4");
			break;
		case EYE_128_DEEPBLUE:
			SetPvrTexture("deepblue-128.pvr4");
			break;
		case EYE_128_BROWN1:
			SetPvrTexture("brown1-128.pvr4");
			break;
		case EYE_128_BROWN2:
			SetPvrTexture("brown2-128.pvr4");
			break;
		case EYE_128_BROWN3:
			SetPvrTexture("brown3-128.pvr4");
			break;
		case EYE_128_RED1:
			SetPvrTexture("red1-128.pvr4");
			break;
		case EYE_128_RED2:
			SetPvrTexture("red2-128.pvr4");
			break;
		case EYE_41:
			SetPvrTexture("eye41-32.pvr4");
			break;
		case EYE_42:
			SetPvrTexture("eye42-32.pvr4");
			break;
		case EYE_43:
			SetPvrTexture("eye43-32.pvr4");
			break;
		case EYE_44:
			SetPvrTexture("eye44-32.pvr4");
			break;
		case EYE_45:
			SetPvrTexture("eye45-32.pvr4");
			break;
		case EYE_46:
			SetPvrTexture("eye46-32.pvr4");
			break;
		case EYE_47:
			SetPvrTexture("eye47-32.pvr4");
			break;
		case EYE_48:
			SetPvrTexture("eye48-32.pvr4");
			break;
		case EYE_49:
			SetPvrTexture("eye49-32.pvr4");
			break;
		case EYE_50:
			SetPvrTexture("eye50-32.pvr4");
			break;
		case EYE_51:
			SetPvrTexture("eye51-32.pvr4");
			break;
#if 0
		case EYE_256_BLUE1:
			SetPvrTexture("blue1-256.pvr4");
			break;
		case EYE_256_BLUE2:
			SetPvrTexture("blue2-256.pvr4");
			break;
		case EYE_256_BLUE3:
			SetPvrTexture("blue3-256.pvr4");
			break;
		case EYE_256_BLUE4:
			SetPvrTexture("blue4-256.pvr4");
			break;
		case EYE_256_BLUE5:
			SetPvrTexture("blue5-256.pvr4");
			break;
		case EYE_256_BLUE6:
			SetPvrTexture("blue6-256.pvr4");
			break;
		case EYE_256_BLUE7:
			SetPvrTexture("blue7-256.pvr4");
			break;
		case EYE_256_BLUE8:
			SetPvrTexture("blue8-256.pvr4");
			break;
		case EYE_256_DEEPBLUE:
			SetPvrTexture("deepblue-256.pvr4");
			break;
		case EYE_256_BROWN1:
			SetPvrTexture("brown1-256.pvr4");
			break;
		case EYE_256_BROWN2:
			SetPvrTexture("brown2-256.pvr4");
			break;
		case EYE_256_BROWN3:
			SetPvrTexture("brown3-256.pvr4");
			break;
		case EYE_256_RED1:
			SetPvrTexture("red1-256.pvr4");
			break;
		case EYE_256_RED2:
			SetPvrTexture("red2-256.pvr4");
			break;
		case EYE_256_GREEN1:
			SetPvrTexture("green1-256.pvr4");
			break;
		case EYE_256_GREEN2:
			SetPvrTexture("green2-256.pvr4");
			break;
		case EYE_256_GREEN3:
			SetPvrTexture("green3-256.pvr4");
			break;
		case EYE_256_GREEN4:
			SetPvrTexture("green4-256.pvr4");
			break;
		case EYE_256_GREEN5:
			SetPvrTexture("green5-256.pvr4");
			break;
		case EYE_256_GREEN6:
			SetPvrTexture("green6-256.pvr4");
			break;
		case EYE_256_GREEN7:
			SetPvrTexture("green7-256.pvr4");
			break;
		case EYE_256_GREEN8:
			SetPvrTexture("green8-256.pvr4");
			break;
		case EYE_256_GREEN9:
			SetPvrTexture("green9-256.pvr4");
			break;
		case EYE_256_PURPLE1:
			SetPvrTexture("purple1-256.pvr4");
			break;
		case EYE_256_PURPLE2:
			SetPvrTexture("purple2-256.pvr4");
			break;
		case EYE_256_PURPLE3:
			SetPvrTexture("purple3-256.pvr4");
			break;
#endif
		default:
			SetPvrTexture("blue7-128.pvr4");
			break;
	}	
}
	
void RenderingEngine::SetNoseTextureData1(GLuint tex) 
{ 
		
    // Set up the texture state.
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glBlendFunc(GL_ONE, GL_SRC_COLOR);
	SetTexture();
	glEnable(GL_BLEND);
	
	return;
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
#if 0
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
	//glShadeModel(GL_SMOOTH);
#endif	
	//SetPngTexture("eye100.png");
	
	
    
   //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	//m_resourceManager->LoadPvrImage("eye100.pvr");
#if 0
	CGImageRef cgimage = (CGImageRef)imageData;
	int width, height;
	void *pixels;
	static unsigned int num=0;
#if 0	
	std::string texFName("eye100.png");
	
	//unsigned int glcomponent, glformat;
	
	m_resourceManager->LoadPngImage(texFName);
	m_resourceManager->GetCGIImage((void **)&cgimage);
#endif		
	int w = CGImageGetWidth((CGImageRef)cgimage);
	int h = CGImageGetHeight((CGImageRef)cgimage);
	width = w;
	height = h;
	if((!w) || (!h)) return;
#if 1
#ifdef MALLOC_DEBUG
	pixels = NTIMalloc( height * width * 4, (char *)__FILE__, __LINE__ );
#else
	pixels = malloc( height * width * 4 );
#endif
	memset(pixels, 0, (height * width * 4));
#else
	pixels = calloc(1, height * width * 4 );
#endif
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context = CGBitmapContextCreate( pixels, width, height, 8, 4 * width, colorSpace, 
														 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );

#if 1
#if 0
	unsigned char* pColor = (unsigned char*) pixels;
	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			unsigned char r = pColor[0];
			unsigned char g = pColor[1];
			unsigned char b = pColor[2];
			unsigned char a = pColor[3];
			a = a ? a : 255;
			float x = (float) a * 1.3f;
			*pColor++ = (unsigned char)((float)(r * 255) / x);
			*pColor++ = (unsigned char)((float)(g * 255) / x);
			*pColor++ = (unsigned char)((float)(b * 255) / x);
			*pColor++ = (unsigned char)x;
		}
	}
#else
	if(m_alphaCorrection) {
		unsigned char* pColor = (unsigned char*) pixels;
		for (int h = 0; h < height; ++h) {
			for (int w = 0; w < width; ++w) {
				unsigned char r = pColor[0];
				unsigned char g = pColor[1];
				unsigned char b = pColor[2];
				unsigned char a = pColor[3];
				a = a ? a : 255;
				*pColor++ = r * 255 / a;
				*pColor++ = g * 255 / a;
				*pColor++ = b * 255 / a;
				*pColor++ = a;
			}
		}
	}
	
#endif
#endif
	
	CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
	//CGContextTranslateCTM( context, 0, h - h );
	//CGContextTranslateCTM(context, 0, height);
	//CGContextScaleCTM(context, 1.0, -1.0);
	//CGContextSetBlendMode(context, kCGBlendModeCopy);
	CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), (CGImage *)cgimage );
#if 0
	int step      = 4;
    int channels  = 1;
	unsigned char *data = (unsigned char*) pixels;
    if (data != NULL) {
        for(int y=0;y<height;y++) {
            for(int x=0;x<width;x++) {
                int intensity = (int)(0.30 * data[(4*y*width)+(4*x)+1] + 
									  0.59 * data[(4*y*width)+(4*x)+2] + 
									  0.11 * data[(4*y*width)+(4*x)+3]);
                
                
                data[y*step+x*channels+0] = intensity;
                //cvdata[y*step+x*channels+1] = data[(4*y*width)+(4*x)+2];
                //cvdata[y*step+x*channels+2] = data[(4*y*width)+(4*x)+1];
            }
        }
    }
	
#endif	
	//m_resourceManager->UnloadImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	glBindTexture(GL_TEXTURE_2D, tex);
#if 0
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
	glShadeModel(GL_SMOOTH);
#else
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
	glShadeModel(GL_SMOOTH);
	//glShadeModel(GL_FLAT);
#endif
	
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	// Enable use of the texture
	glEnable(GL_TEXTURE_2D);
	
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	num = (num + 1)%(0xffffff);
	// Enable blending
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#ifdef MALLOC_DEBUG
	NTIFree(pixels, (char *)__FILE__, __LINE__);
#else	
	free(pixels);
#endif
	
	//NTIMallocCheck((char *)__FILE__, __LINE__);
	CGColorSpaceRelease( colorSpace );
	CGContextRelease(context);
	return;

		
}

	
// This is not a fully generalized image loader. It is an example of how to use
// CGImage to directly access decompressed image data. Only the most commonly
// used image formats are supported. It will be necessary to expand this code
// to account for other uses, for example cubemaps or compressed textures.
//
// If the image format is supported, this loader will Gen a OpenGL 2D texture object
// and upload texels from it, padding to POT if needed. For image processing purposes,
// border pixels are also replicated here to ensure proper filtering during e.g. blur.
//
// The caller of this function is responsible for deleting the GL texture object.
//void loadTexture(const char *name, Image *img, RendererInfo *renderer)
void RenderingEngine::LoadTexture(GLuint texID) 
{
	GLuint components;
	//GLuint x, y;
	GLuint imgWide, imgHigh;      // Real image size
	GLuint rowBytes, rowPixels;   // Image size padded by CGImage
	GLuint POTWide, POTHigh;      // Image size padded to next power of two
	CGBitmapInfo info;            // CGImage component layout info
	CGColorSpaceModel colormodel; // CGImage colormodel (RGB, CMYK, paletted, etc)
	GLenum internal, format;
	GLubyte *pixels, *temp = NULL;
	
	CGImageRef CGImage = (CGImageRef)imageData;
	int width, height;
	
	int w = CGImageGetWidth((CGImageRef)CGImage);
	int h = CGImageGetHeight((CGImageRef)CGImage);
	width = w;
	height = h;
	if((!w) || (!h)) return;
	
	// Parse CGImage info
	info       = CGImageGetBitmapInfo(CGImage);		// CGImage may return pixels in RGBA, BGRA, or ARGB order
	colormodel = CGColorSpaceGetModel(CGImageGetColorSpace(CGImage));
	size_t bpp = CGImageGetBitsPerPixel(CGImage);
	if (bpp < 8 || bpp > 32 || (colormodel != kCGColorSpaceModelMonochrome && colormodel != kCGColorSpaceModelRGB))
	{
		// This loader does not support all possible CGImage types, such as paletted images
		//xCGImageRelease(CGImage);
		return;
	}
	components = bpp>>3;
	rowBytes   = CGImageGetBytesPerRow(CGImage);	// CGImage may pad rows
	rowPixels  = rowBytes / components;
	imgWide    = CGImageGetWidth(CGImage);
	imgHigh    = CGImageGetHeight(CGImage);
#if 0
	img->wide  = rowPixels;
	img->high  = imgHigh;
	img->s     = (float)imgWide / rowPixels;
	img->t     = 1.0;
#endif	
	// Choose OpenGL format
	switch(bpp)
	{
		default:
			//rt_assert(0 && "Unknown CGImage bpp");
		case 32:
		{
			internal = GL_RGBA;
			switch(info & kCGBitmapAlphaInfoMask)
			{
				case kCGImageAlphaPremultipliedFirst:
				case kCGImageAlphaFirst:
				case kCGImageAlphaNoneSkipFirst:
					format = GL_BGRA;
					break;
				default:
					format = GL_RGBA;
			}
			break;
		}
		case 24:
			internal = format = GL_RGB;
			break;
		case 16:
			internal = format = GL_LUMINANCE_ALPHA;
			break;
		case 8:
			internal = format = GL_LUMINANCE;
			break;
	}
	
	// Get a pointer to the uncompressed image data.
	//
	// This allows access to the original (possibly unpremultiplied) data, but any manipulation
	// (such as scaling) has to be done manually. Contrast this with drawing the image
	// into a CGBitmapContext, which allows scaling, but always forces premultiplication.
	CGDataProviderRef dp = CGImageGetDataProvider (CGImage);
//	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(CGImage));
	CFDataRef data = CGDataProviderCopyData(dp);
	//rt_assert(data);
	pixels = (GLubyte *)CFDataGetBytePtr(data);
	//rt_assert(pixels);
	
	// If the CGImage component layout isn't compatible with OpenGL, fix it.
	// On the device, CGImage will generally return BGRA or RGBA.
	// On the simulator, CGImage may return ARGB, depending on the file format.
	if (format == GL_BGRA)
	{
		uint32_t *p = (uint32_t *)pixels;
		//int i, num = img->wide * img->high;
		int i, num = width * height;
		
		if ((info & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host)
		{
			// Convert from ARGB to BGRA
			for (i = 0; i < num; i++)
				p[i] = (p[i] << 24) | ((p[i] & 0xFF00) << 8) | ((p[i] >> 8) & 0xFF00) | (p[i] >> 24);
		}
		
		// All current iPhoneOS devices support BGRA via an extension.
		if (!renderer.extension[IMG_texture_format_BGRA8888])
		{
			format = GL_RGBA;
			
			// Convert from BGRA to RGBA
			for (i = 0; i < num; i++)
#if __LITTLE_ENDIAN__
				p[i] = ((p[i] >> 16) & 0xFF) | (p[i] & 0xFF00FF00) | ((p[i] & 0xFF) << 16);
#else
			p[i] = ((p[i] & 0xFF00) << 16) | (p[i] & 0xFF00FF) | ((p[i] >> 16) & 0xFF00);
#endif
		}
	}
	
	// Determine if we need to pad this image to a power of two.
	// There are multiple ways to deal with NPOT images on renderers that only support POT:
	// 1) scale down the image to POT size. Loses quality.
	// 2) pad up the image to POT size. Wastes memory.
	// 3) slice the image into multiple POT textures. Requires more rendering logic.
	//
	// We are only dealing with a single image here, and pick 2) for simplicity.
	//
	// If you prefer 1), you can use CoreGraphics to scale the image into a CGBitmapContext.
	//POTWide = nextPOT(img->wide);
	//POTHigh = nextPOT(img->high);
	POTWide = width;
	POTHigh = height;
#if 0	
	if (!renderer.extension[APPLE_texture_2D_limited_npot] && (img->wide != POTWide || img->high != POTHigh))
	{
		GLuint dstBytes = POTWide * components;
		GLubyte *temp = (GLubyte *)malloc(dstBytes * POTHigh);
		
		for (y = 0; y < img->high; y++)
			memcpy(&temp[y*dstBytes], &pixels[y*rowBytes], rowBytes);
		
		img->s *= (float)img->wide/POTWide;
		img->t *= (float)img->high/POTHigh;
		img->wide = POTWide;
		img->high = POTHigh;
		pixels = temp;
		rowBytes = dstBytes;
	}
	
	// For filters that sample texel neighborhoods (like blur), we must replicate
	// the edge texels of the original input, to simulate CLAMP_TO_EDGE.
	{
		GLuint replicatew = MIN(MAX_FILTER_RADIUS, img->wide-imgWide);
		GLuint replicateh = MIN(MAX_FILTER_RADIUS, img->high-imgHigh);
		GLuint imgRow = imgWide * components;
		
		for (y = 0; y < imgHigh; y++)
			for (x = 0; x < replicatew; x++)
				memcpy(&pixels[y*rowBytes+imgRow+x*components], &pixels[y*rowBytes+imgRow-components], components);
		for (y = imgHigh; y < imgHigh+replicateh; y++)
			memcpy(&pixels[y*rowBytes], &pixels[(imgHigh-1)*rowBytes], imgRow+replicatew*components);
	}
#endif	
//	if (img->wide <= renderer.maxTextureSize && img->high <= renderer.maxTextureSize)
	if (imgWide <= renderer.maxTextureSize && imgHigh <= renderer.maxTextureSize)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		// Set filtering parameters appropriate for this application (image processing on screen-aligned quads.)
		// Depending on your needs, you may prefer linear filtering, or mipmap generation.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexImage2D(GL_TEXTURE_2D, 0, internal, img->wide, img->high, 0, format, GL_UNSIGNED_BYTE, pixels);
		glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	}
	
	if (temp) free(temp);
	CFRelease(data);
	//CGImageRelease(CGImage);
	//img->texID = texID;
#endif
}

void RenderingEngine::SetPngTexture(const string& filename) const
{
	TextureDescription description = m_resourceManager->LoadPngImage(filename);
	//TextureDescription description = m_resourceManager->GenerateCircle();
	
	GLenum format;
	switch (description.Format) {
		case TextureFormatGray:      format = GL_LUMINANCE;       break;
		case TextureFormatGrayAlpha: format = GL_LUMINANCE_ALPHA; break;
		case TextureFormatRgb:       format = GL_RGB;             break;
		case TextureFormatRgba:      format = GL_RGBA;            break;
	}
	
	GLenum type;
	switch (description.BitsPerComponent) {
		case 8: type = GL_UNSIGNED_BYTE; break;
		case 4:
			if (format == GL_RGBA) {
				type = GL_UNSIGNED_SHORT_4_4_4_4;
				break;
			}
			// intentionally fall through
		default:
			assert(!"Unsupported format.");
	}
	
	void* data = m_resourceManager->GetImageData();
	ivec2 size = description.Size;
	glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, type, data);
	m_resourceManager->UnloadImage();
}
	
void RenderingEngine::SetPvrTexture(const string& filename) const
{
	//if(!m_surfaceType) {
	if(1) {
	TextureDescription description = m_resourceManager->LoadPvrImage(filename);
	unsigned char* data = (unsigned char*) m_resourceManager->GetImageData();
	int width = description.Size.x;
	int height = description.Size.y;
	
	int bitsPerPixel;
	GLenum format;
	bool compressed = false;
	
	switch (description.Format) {
        case TextureFormatPvrtcRgba2:
        case TextureFormatPvrtcRgb2:
        case TextureFormatPvrtcRgba4:
        case TextureFormatPvrtcRgb4:
            compressed = true;
            break;
    }
    
    if (!compressed) {
        GLenum type;
        switch (description.Format) {
            case TextureFormatRgba:
                assert(description.BitsPerComponent == 4);
                format = GL_RGBA;
                type = GL_UNSIGNED_SHORT_4_4_4_4;
                bitsPerPixel = 16;
                break;
            case TextureFormat565:
                format = GL_RGB;
                type = GL_UNSIGNED_SHORT_5_6_5;
                bitsPerPixel = 16;
                break;
            case TextureFormat5551:
                format = GL_RGBA;
                type = GL_UNSIGNED_SHORT_5_5_5_1;
                bitsPerPixel = 16;
                break;
        }
        for (int level = 0; width > 0 && height > 0; ++level) {
            GLsizei size = width * height * bitsPerPixel / 8;
            glTexImage2D(GL_TEXTURE_2D, level, format, width, 
                         height, 0, format, type, data);
            data += size;
            width >>= 1; height >>= 1;
        }
		
        m_resourceManager->UnloadImage();
        return;
    }
	switch (description.Format) {
		case TextureFormatPvrtcRgba2:
			bitsPerPixel = 2;
			format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			break;
		case TextureFormatPvrtcRgb2:
			bitsPerPixel = 2;
			format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			break;
		case TextureFormatPvrtcRgba4:
			bitsPerPixel = 4;
			format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			break;
		case TextureFormatPvrtcRgb4:
			bitsPerPixel = 4;
			format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			break;
		default:
			compressed = false;
			break;
	}
	
	if (compressed) {
		for (int level = 0; level < description.MipCount; ++level) {
			GLsizei size = max(32, width * height * bitsPerPixel / 8);
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, data);
			data += size;
			width >>= 1; height >>= 1;
		}
	} else {
		GLenum type;
		switch (description.Format) {
			case TextureFormatRgba:
				assert(description.BitsPerComponent == 4);
				format = GL_RGBA;
				type = GL_UNSIGNED_SHORT_4_4_4_4;
				bitsPerPixel = 16;
				break;
			case TextureFormat565:
				format = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				bitsPerPixel = 16;
				break;
			case TextureFormat5551:
				format = GL_RGBA;
				type = GL_UNSIGNED_SHORT_5_5_5_1;
				bitsPerPixel = 16;
				break;
		}
		for (int level = 0; level < description.MipCount; ++level) {
			glTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, format, type, data);
			GLsizei size = width * height * bitsPerPixel / 8;
			data += size;
			width >>= 1; height >>= 1;
		}
	}
	
	m_resourceManager->UnloadImage();
	}
	else {
	TextureDescription description = m_resourceManager->LoadPvrImage(filename);
	unsigned char* data = (unsigned char*) m_resourceManager->GetImageData();
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 256, 256, 0, (256 * 256) / 2, data);
	}
}

void RenderingEngine::SetMatProperty(int p, float value) 
{
	float alpha = 1.0;
	float val = (value/100.0f);
	if(m_varyAlpha) alpha = val;
	m_matPropertyEnabled = TRUE;
	switch (p) {
		case AMBIENT:
			m_ambient = vec4(val, val, val, alpha);
			break;
		case DIFFUSE:
			m_diffuse = vec4(val, val, val, alpha);
			break;
		case SPECULAR:
			m_specular = vec4(val, val, val, alpha);
			break;
		case SHININESS:
			m_shininess = val;
			break;
		case ALPHA:
			m_varyAlpha = value;
			break;
		case NONE: /* RESET */
			m_matPropertyEnabled = FALSE;
			m_ambient		= vec4(0.2, 0.2, 0.2, 1.0);
			m_diffuse		= vec4(1.0, 1.0, 1.0, 1.0);
			m_specular		= vec4(1.0, 1.0, 1.0, 1.0);
			m_shininess		= 0.0;			
			break;
		default:
			break;
	}
	
	return;
}

void RenderingEngine::DeallocCurrentObjectOLD() 
{
	vector<Group *>::iterator itr;
#if 0
	//int n = m_currentSurface;
	
	vec3 *vert = m_vertices;
	vec3 *vertNormals = m_vertexNormals;
	vec3 *surfaceNormals = m_surfaceNormals;
	float *texCoords = m_textureCoords;
	m_currentSurface = -1;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++)
	{
		
#ifdef MALLOC_DEBUG
		NTIFree((void *)(*itr), (char *)__FILE__, __LINE__);
#else
		free((void *)(*itr));
#endif
	}
	m_groups.clear();
#ifdef MALLOC_DEBUG
	NTIFree(m_vertices, (char *)__FILE__, __LINE__);
	NTIFree(m_vertexNormals, (char *)__FILE__, __LINE__);
	NTIFree(m_surfaceNormals, (char *)__FILE__, __LINE__);
	NTIFree(m_textureCoords, (char *)__FILE__, __LINE__);
	
	//NTIMallocCheck((char *)__FILE__, __LINE__);
#else	
	free(vert);
	free(vertNormals);
	free(surfaceNormals);
	free(texCoords);
#endif	
	

	m_currentSurface = -1;
	for(itr=m_groups.begin(); itr != m_groups.end(); itr++) {
		Group *group = (*itr);
		glDeleteTextures(1, &(group->texture[0]));
	}
#endif
	return;
}
void RenderingEngine::DeallocCurrentObject() 
{
	vector<Group *>::iterator itr;
	m_currentSurface = -1;
	for(int i=0; i < SurfaceCount; i++) {
		for(itr=m_groups[i].begin(); itr != m_groups[i].end(); itr++) {
			Group *group = (*itr);
			glDeleteTextures(1, &(group->texture[0]));
		}
	}
	return;
}

void RenderingEngine::ToggleLight(int val)
{
	int light=0;
	switch(val) {
		case L0:
			light = GL_LIGHT0;
			break;
		case L1:
			light = GL_LIGHT1;
			break;
		case L2:
			light = GL_LIGHT2;
			break;
		case L3:
			light = GL_LIGHT3;
			break;
		case L4:
			light = GL_LIGHT4;
			break;
		default:
			return;
	}
	
	if((val >= 0) && (val <= 4)) {
		if(m_lightState[val]) {
			m_lightState[val]=0;
			glDisable(light);
		}
		else {
			m_lightState[val]=1;
			glEnable(light);
		}
	}
	return;
}

void RenderingEngine::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
}
	
}
