#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "Interfaces.hpp"
#include "Matrix.hpp"
#include <iostream>
#import <QuartzCore/QuartzCore.h>
#import "ConstantsAndMacros.h"

namespace SolidES2 {


const int SurfaceCount = 5;

#define STRINGIFY(A)  #A
//#include "../Shaders/SimpleLighting.es2.vert"
//#include "../Shaders/SimpleLighting.es2.frag"

struct UniformHandles {
    GLuint Modelview;
    GLuint Projection;
    GLuint NormalMatrix;
    GLuint LightPosition;
	GLuint Light2Position;
    GLint AmbientMaterial;
    GLint SpecularMaterial;
    GLint Shininess;
};
    
struct AttributeHandles {
    GLint Position;
    GLint Normal;
    GLint DiffuseMaterial;
};
    
struct Drawable {
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    int IndexCount;
};

class RenderingEngine : public IRenderingEngine {
public:
    RenderingEngine();
	RenderingEngine(IResourceManager* resourceManager);
    void Initialize(const vector<ISurface*>& surfaces);
	void Initialize1(const vector<ISurface*>& surfaces);
    void Render(const vector<Visual>& visuals) ;
	void Render1(const vector<Visual>& visuals) {};
	bool PresentBuffer() {return m_presentBuffer;};
	void SetNoseTextureData(void *dat, int w, int h) {};
	void SetMatProperty(int p, float value) {};
private:
	bool m_presentBuffer;
    GLuint BuildShader(const char* source, GLenum shaderType) const;
    GLuint BuildProgram(const char* vShader, const char* fShader) const;
	vector<vec4> m_surfaceBoundingSphere;
    vector<Drawable> m_drawables;
	vector<Group *>		m_groups[SurfaceCount];
	vec3 *m_vertices[SurfaceCount];
	vec3 *m_vertexNormals[SurfaceCount];
	vec3 *m_surfaceNormals[SurfaceCount];
	unsigned int m_numberOfVertices[SurfaceCount];
	float *m_textureCoords[SurfaceCount];
	unsigned int m_valuesPerCoord[SurfaceCount];
	unsigned int m_textureCoordsCount[SurfaceCount];
	GLuint m_frameBuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
    mat4 m_translation;
	GLint m_backingWidth;
	GLint m_backingHeight;
    UniformHandles m_uniforms;
    AttributeHandles m_attributes;
	void *imageData;
	int imageWidth;
	int imageHeight;
	vec4 m_ambient;
	vec4 m_diffuse;
	vec4 m_specular;
	float m_shininess;
	int m_matPropertyEnabled;
	int m_varyAlpha;	
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
    glGenRenderbuffers(1, &m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
}

RenderingEngine::RenderingEngine(IResourceManager* resourceManager)
{
	/* OpenGL 2.0 omits the OES suffix */
	m_resourceManager = resourceManager;
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glGenRenderbuffers(1, &m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	
	m_ambient		= vec4(0.2, 0.2, 0.2, 1.0);
	m_diffuse		= vec4(1.0, 1.0, 1.0, 1.0);
	m_specular		= vec4(1.0, 1.0, 1.0, 1.0);
	m_shininess		= 0.0;
	
	m_matPropertyEnabled = FALSE;
	m_varyAlpha = TRUE;
	
}
	
void RenderingEngine::Initialize(const vector<ISurface*>& surfaces)
{
    vector<ISurface*>::const_iterator surface;
    for (surface = surfaces.begin(); surface != surfaces.end(); ++surface) {
        
        // Create the VBO for the vertices.
        vector<float> vertices;
        (*surface)->GenerateVertices(vertices, VertexFlagsNormals);
        GLuint vertexBuffer;
		size_t sz = vertices.size();
		size_t sz1 = sizeof(vertices[0]);
		size_t sz0 = sz * sz1;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices[0]),
                     &vertices[0],
                     GL_STATIC_DRAW);
        
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
    
    // Extract width and height.
    int width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                 GL_RENDERBUFFER_HEIGHT, &height);
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
	//Set up some default material parameters.
	glUniform3f(m_uniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
	glUniform3f(m_uniforms.SpecularMaterial, 0.5, 0.5, 0.5);
	glUniform1f(m_uniforms.Shininess, 50);

    // Initialize various state.
    glEnableVertexAttribArray(m_attributes.Position);
    glEnableVertexAttribArray(m_attributes.Normal);
    glEnable(GL_DEPTH_TEST);

    // Set up transforms.
    m_translation = mat4::Translate(0, 0, -7);
	//m_translation = mat4::Translate(0, 0, 0);
}

void RenderingEngine::Initialize1(const vector<ISurface*>& surfaces)
{
	vector<ISurface*>::const_iterator surface;
	int n=0;
	
	const GLfloat			lightAmbient[] = {0.2, 0.2, 0.2, 1.0};
	const GLfloat			lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
		
	const GLfloat			lightPosition[] = {5.0, 5.0, 15.0, 0.0}; 
	const GLfloat			light2Position[] = {-5.0, -5.0, 15.0, 0.0};
	const GLfloat			lightShininess = 0.0;
	
	
		
		
		
#if 0
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 
	glLightfv(GL_LIGHT0, GL_SHININESS, &lightShininess);
	
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light2Position); 
	glLightfv(GL_LIGHT1, GL_SHININESS, &lightShininess);	
#endif	
	//int maxTextureUnits[] = new int[1];
	// glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, maxTextureUnits);
	//int nbTextureUnits = maxTextureUnits[0];
	
	for (surface = surfaces.begin(); surface != surfaces.end(); ++surface, n++) {
		
		// Create the VBO for the vertices.
		vector<float> vertices1;
		(*surface)->GenerateVertices(vertices1, VertexFlagsNormals);
		
		vec3	*surfaceNormals;
		vec3	*vertexNormals;
		unsigned int numberOfFaces=0;
		
		(*surface)->GetGroupAndVertices(&m_groups[n], &m_vertices[n], &m_numberOfVertices[n], &numberOfFaces, 
										&surfaceNormals, &vertexNormals,
										&m_textureCoords[n], &m_valuesPerCoord[n], &m_textureCoordsCount[n]);
		m_vertexNormals[n] = vertexNormals;
		m_surfaceNormals[n] = surfaceNormals;
		vec4 sphere = (*surface)->GetBoundingSphere();
		m_surfaceBoundingSphere.push_back(sphere);
		
		
		
		vector<Group *> ::iterator itr;
		
		
		for(itr = m_groups[n].begin(); itr != m_groups[n].end(); itr++) {
			
			Group *group = *itr;
			
			if(group->oneTexture) {
				glGenTextures(1, &group->texture[0]);
				glBindTexture(GL_TEXTURE_2D, group->texture[0]);
				
			}
			else {
				glGenTextures(1, &group->material->texture->texture[0]);
				glBindTexture(GL_TEXTURE_2D, group->material->texture->texture[0]);
				
				//SetNoseTextureData1(group->material->texture->texture[0]) ;
			}
		}
				
	}
	

	// Extract width and height from the color buffer.
	glGetRenderbufferParameteriv(GL_RENDERBUFFER,
									GL_RENDERBUFFER_WIDTH, &m_backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER,
									GL_RENDERBUFFER_HEIGHT, &m_backingHeight);
	
	// Create a depth buffer that has the same size as the color buffer.
	glGenRenderbuffers(1, &m_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
							 m_backingWidth, m_backingHeight);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
								 GL_RENDERBUFFER, m_colorRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								 GL_RENDERBUFFER, m_depthRenderbuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
			
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		//cout << "failed to make complete framebuffer object %x" <<  status << endl;
		//return NO;
		
		
	}
	
	//Set up some default material parameters.
	glUniform4f(m_uniforms.AmbientMaterial, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(m_uniforms.SpecularMaterial, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(m_uniforms.LightPosition, 5.0f, 5.0f, 15.0f, 0.0f);
	glUniform4f(m_uniforms.Light2Position, -5.0f, -5.0f, 15.0f, 0.0f);
	glUniform1f(m_uniforms.Shininess, 50);
	
	// Set up various GL state.
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	glEnableVertexAttribArray(m_attributes.Position);
	glEnableVertexAttribArray(m_attributes.Normal);
#if 0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
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
			
	return;
		
}
	
	
void RenderingEngine::Render(const vector<Visual>& visuals) 
{
	m_presentBuffer = false;
    //glClearColor(0.5f, 0.75f, 0.55f, 1);
	// glClearColor(0.5f, 0.5f, 0.5f, 1);
	 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) {

		float myScale=1.0f; //0.2f
		mat4 scale = mat4::Scale(myScale);
		
        // Set the viewport transform.
        ivec2 size = visual->ViewportSize;
		
        ivec2 lowerLeft = visual->LowerLeft;
        //glViewport(lowerLeft.x, lowerLeft.y, (GLsizei)((float)size.x * myScale), (GLsizei)((float)size.y * myScale));
        glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y );

        // Set the light position.
        vec4 lightPosition(0.25, 0.25, 1, 0);
        glUniform3fv(m_uniforms.LightPosition, 1, lightPosition.Pointer());

        // Set the model-view transform.
        mat4 rotation = visual->Orientation.ToMatrix();
        mat4 modelview = rotation * m_translation;
		if((visualIndex ==0)  && (myScale != 1.0f)) {
			
			mat4 modelview1 = scale * modelview ;
			mat4 myT = mat4::Translate(0, -20.75, 1);
			mat4 modelview2 = myT * modelview1;
			glUniformMatrix4fv(m_uniforms.Modelview, 1, 0, modelview2.Pointer());
			
		}
		else
			glUniformMatrix4fv(m_uniforms.Modelview, 1, 0, modelview.Pointer());
        
        // Set the normal matrix.
        // It's orthogonal, so its Inverse-Transpose is itself!
        mat3 normalMatrix = modelview.ToMat3();
        glUniformMatrix3fv(m_uniforms.NormalMatrix, 1, 0, normalMatrix.Pointer());

        // Set the projection transform.
        float h = 4.0f * size.y / size.x;
        mat4 projectionMatrix = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);
		//mat4 projectionMatrix = mat4::Frustum(-2, 2, -h / 2, h / 2, 0.01, 100);
		//const GLfloat zNear = 0.01, zFar = 1000.0, fieldOfView = 45.0; 
	
		//const float myH=480.0f, myW=320.0f;
		//GLfloat mysize = zNear * tanf((fieldOfView /180 * M_PI) / 2.0); 
		//mat4 projectionMatrix = mat4::Frustum(-mysize, mysize, -mysize / (myW / myH), mysize / (myW / myH), zNear, zFar);
        glUniformMatrix4fv(m_uniforms.Projection, 1, 0, projectionMatrix.Pointer());
        
        // Set the diffuse color.
      //  vec3 color = visual->Color * 0.75f;
       // glVertexAttrib4f(m_attributes.DiffuseMaterial, color.x, color.y, color.z, 1);
        
		
        // Draw the surface.
        int stride = 2 * sizeof(vec3);
        const GLvoid* offset = (const GLvoid*) sizeof(vec3);
        GLint position = m_attributes.Position;
        GLint normal = m_attributes.Normal;
        const Drawable& drawable = m_drawables[visualIndex];
        glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.IndexBuffer);
        glDrawElements(GL_TRIANGLES, drawable.IndexCount, GL_UNSIGNED_SHORT, 0);
		m_presentBuffer= true;
    }
}

GLuint RenderingEngine::BuildShader(const char* source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return shaderHandle;
}

GLuint RenderingEngine::BuildProgram(const char* vertexShaderSource,
                                      const char* fragmentShaderSource) const
{
    GLuint vertexShader = BuildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = BuildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return programHandle;
}

#if 0
void RenderingEngine::Render1(const vector<Visual>& visuals) 
{

	m_presentBuffer=false;
	//glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	vector<Visual>::const_iterator visual = visuals.begin();
	for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) {
		
		vec4 sphere = m_surfaceBoundingSphere[visualIndex];
		
		vec2 screenPos = visual->ScreenPos;
		if((screenPos.x == -1000.0f) || (screenPos.y == -1000.0f)) continue;
		
		vector<Group *> ::iterator itr;
		// Set the viewport transform.
		ivec2 size = visual->ViewportSize;
		ivec2 lowerLeft = visual->LowerLeft;
		
		lowerLeft.x = (int)screenPos.x;
		lowerLeft.y = (int)screenPos.y;
		glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
		
		GLfloat aspect = (float)(size.x)/(float)(size.y);
		
		// Set the light position.
		//vec4 lightPosition(0.25, 0.25, 1, 0);
		//glUniform3fv(m_uniforms.LightPosition, 1, lightPosition.Pointer());
		
		// Set the model-view transform.
		mat4 rotation = visual->Orientation.ToMatrix();
		
		GLfloat diam = 2 * sphere.w;
#if 0
		GLfloat h = 4.0f * size.y / size.x;
		GLfloat left = sphere.x - diam;
		GLfloat right = sphere.x + diam;
		GLfloat bottom = sphere.y - diam;
		GLfloat top = sphere.y + diam;
#endif
		GLfloat zNearF = 0.001f;
		GLfloat zNearO = 1.0f;
		GLfloat zFarO = zNearO+diam;
		//GLfloat zFar = 1000.0f;
		GLfloat zFarF = zNearF+diam;
		GLfloat rMax = 0.0f;
		
		mat4 modelview = rotation * m_translation;
		mat4 m_translation1 = mat4::Translate(0, 0, -diam);
		modelview = rotation * m_translation1;
		
		
		glUniformMatrix4fv(m_uniforms.Modelview, 1, 0, modelview.Pointer());
		const GLfloat fieldOfView = 45.0;
		
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
		
		//glMatrixMode(GL_PROJECTION);
		//glLoadMatrixf(projection.Pointer());
		glUniformMatrix4fv(m_uniforms.Projection, 1, 0, projection.Pointer());
		
		// +++lal
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		m_presentBuffer = true;
		// Draw the surface.
		//int stride = 2 * sizeof(vec3);
		//const GLvoid* normalOffset = (const GLvoid*) sizeof(vec3);
		//const DrawableTexture& drawable = m_drawablesT[visualIndex];
		//for(int i=0; i < drawable.VertexBuffer.size(); i++) {
		//	glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer[i]);
		//}
#if 0
		// Enable and load the vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_vertices[visualIndex]); 
		glNormalPointer(GL_FLOAT, 0, m_vertexNormals[visualIndex]);
#else
		GLint position = m_attributes.Position;
        GLint normal = m_attributes.Normal;
        int stride = 2 * sizeof(vec3);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
#endif
		
		
		// Loop through each group
		//vector<Group *> ::iterator itr;
#if 1
		//glMatrixMode(GL_TEXTURE);
		if (m_textureCoords[visualIndex] != NULL)
		{
			//glClientActiveTexture(GL_TEXTURE0+texUnit);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			//m_valuesPerCoord[visualIndex] =2;
			//cout << "vi = " << visualIndex << " m_valuesPerCoord = " << m_valuesPerCoord[visualIndex] << endl;
			glTexCoordPointer(m_valuesPerCoord[visualIndex], GL_FLOAT, 0, &m_textureCoords[visualIndex][0]);
		}
#endif
		
		//GLuint textureFrameBuffer;
		
		//for(int p=0; p < 2; p++) {
		for(itr = m_groups[visualIndex].begin(); itr != m_groups[visualIndex].end(); itr++)
		{
			
			Group *group = *itr;
			
			if ((!group->oneTexture) && (group->material) && (m_textureCoords[visualIndex] != NULL) && (group->material->texture != NULL)) {
				
				//glEnable(GL_TEXTURE_2D);
				
				SetNoseTextureData1(group->material->texture->texture[0]);
				
				
				//texUnit++;
			}
			else if(group->oneTexture) {
				
				SetNoseTextureData1(group->texture[0]);
			}
			
			// clear buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glColor4f(0.001f, 0.001f, 0.001f, 0.001f);
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
				cout << "Erroraaaaaa. glError: " << err << endl;
			
			
#if 1			
			
			if(m_matPropertyEnabled == TRUE) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const GLfloat *)&m_ambient);
				glColor4f(m_diffuse.x, m_diffuse.y, m_diffuse.z, m_diffuse.w);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  (const GLfloat *)&m_diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const GLfloat *)&m_specular);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);
			}
			
#endif
			for (int i=0; i < group->numberOfFaces; i++) {
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, &(group->faces[i]));
			}
			//glDrawElements(GL_TRIANGLES, 3*group->numberOfFaces, GL_UNSIGNED_SHORT, &(group->faces[0]));
			
			
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

void RenderingEngine::SetNoseTextureData(void *dat, int w, int h) 
{ 
	
	imageData =dat; 
	imageWidth =w; 
	imageHeight=h;
	
	return;
	
	
}

void RenderingEngine::SetNoseTextureData1(GLuint tex) 
{ 
	
	CGImageRef cgimage = (CGImageRef)imageData;
	int width, height;
	void *pixels;
	static unsigned int num=0;
#if 0	
	std::string texFName("nosetex.png");
	
	unsigned int glcomponent, glformat;
	
	m_resourceManager->LoadPngImage(texFName);
	m_resourceManager->GetCGIImage((void **)&cgimage);
#endif		
	int w = CGImageGetWidth((CGImageRef)cgimage);
	int h = CGImageGetHeight((CGImageRef)cgimage);
	width = w;
	height = h;
	if((!w) || (!h)) return;
#if 1
	pixels = malloc( height * width * 4 );
	memset(pixels, 0, (height * width * 4));
#else
	pixels = calloc(1, height * width * 4 );
#endif
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context = CGBitmapContextCreate( pixels, width, height, 8, 4 * width, colorSpace, 
												 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
	
#if 0
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
#endif
	
	CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
	//CGContextTranslateCTM( context, 0, h - h );
	CGContextTranslateCTM(context, 0, height);
	CGContextScaleCTM(context, 1.0, -1.0);
	CGContextSetBlendMode(context, kCGBlendModeCopy);
	CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), (CGImage *)cgimage );
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
	
	free(pixels);
	CGColorSpaceRelease( colorSpace );
	CGContextRelease(context);
	return;
	
	
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
#endif
    
}
