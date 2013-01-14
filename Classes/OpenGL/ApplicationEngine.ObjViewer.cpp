#include "Interfaces.hpp"
#include "ObjSurface.hpp"
#include "ParametricEquations.hpp"
#include <assert.h>
#include <iostream>
#include "ConstantsAndMacros.h"

using namespace std;

namespace ObjViewer {

static const int ButtonCount = SurfaceCount - 1;

struct Animation {
    bool Active;
    float Elapsed;
    float Duration;
    Visual StartingVisuals[SurfaceCount];
    Visual EndingVisuals[SurfaceCount];
};

class ApplicationEngine : public IApplicationEngine {
public:
    ApplicationEngine(IRenderingEngine* renderingEngine, IResourceManager* resourceManager);
    //~ApplicationEngine();
    void Initialize(int width, int height);
    void OnFingerUp(vec2 location);
    void OnFingerDown(vec2 location);
    void OnFingerMove(vec2 oldLocation, vec2 newLocation);
	void OnTwoFingerUp(vec2 loc, vec2 loc1);
    void OnTwoFingerDown(vec2 loc, vec2 loc1);
    void OnTwoFingerMove(vec2 loc, vec2 loc1);	
    void Render() ;
    void UpdateAnimation(float dt);
	int SetupScreenPos(int surfaceIndex, float x, float y, float z, float w);
	int BrowseObjects(int);
	void GetSurfaceScreenPos(int surfaceIndex, float *x, float *y, float *z, float *w);
	void MoveObject() {m_move=true; m_spinning=false; m_resize=false;};
	void RotateObject() {m_move=false; m_spinning=true; m_resize=false;};
	void ResizeObject() {m_move=false; m_spinning=false; m_resize=true;};
	bool PresentBuffer();
	void ResetMovement();
	void PreserveOrientation(int val);
	void SetZoomFactor(float val);
private:
    void PopulateVisuals(Visual* visuals) const;
    int MapToSurface(vec2 touchpoint) const;
    vec3 MapToSphere(vec2 touchpoint) const;
	vec2 MapToViewport(vec2 touchpoint, int surfaceIndex) const;
	string GetObjectName(int noseIndex) const;
	void InitializeObjectOLD();
	void InitializeObject();
	vector<ISurface*> m_surfaces;
    float m_trackballRadius;
    ivec2 m_screenSize;
    vec2 m_centerPoint;
    vec2 m_fingerStart;
	vec2 m_fingerEnd;
    bool m_spinning;
	bool m_move;
	bool m_resize;
	float m_scaleFactor;
	Quaternion m_orientation[MaxEyeSurfaceCount];
	Quaternion m_previousOrientation[MaxEyeSurfaceCount];
    int m_currentSurface;
    ivec2 m_buttonSize;
    int m_pressedButton;
    float m_initialDistanceBetweenFingers;
	int m_adjustForWideAngles;
	vec4 m_objscreenpos[SurfaceCount];
    Animation m_animation;
	string m_path;
	int m_preserveOrientation;
    IRenderingEngine* m_renderingEngine;
    IResourceManager* m_resourceManager;
};
    
IApplicationEngine* CreateApplicationEngine(IRenderingEngine* renderingEngine, IResourceManager* resourceManager)
{
    return new ApplicationEngine(renderingEngine, resourceManager);
}

ApplicationEngine::ApplicationEngine(IRenderingEngine* renderingEngine, IResourceManager* resourceManager) :
    m_spinning(false),
	m_move(false),
	m_resize(false),
    m_pressedButton(-1),
    m_renderingEngine(renderingEngine),
    m_resourceManager(resourceManager)
{
    m_animation.Active = false;
	for(int i=0; i < SurfaceCount; i++) {
		m_objscreenpos[i].x=0.0f;
		m_objscreenpos[i].y=0.0f;
		m_objscreenpos[i].z=0.0f;
		m_objscreenpos[i].w=0.0f;
	}
}

//ApplicationEngine::~ApplicationEngine()
//{
//    delete m_renderingEngine;
//}

void ApplicationEngine::Initialize(int width, int height)
{
    m_trackballRadius = width / 3;
    //m_buttonSize.y = height / 10;
	m_buttonSize.y = height / 40;
    m_buttonSize.x = 4 * m_buttonSize.y / 3;
    //m_screenSize = ivec2(width, height - m_buttonSize.y);
	m_screenSize = ivec2(width, height);
    m_centerPoint = vec2(m_screenSize.x / 2,m_screenSize.y / 2) ;
    m_scaleFactor = 1.0f;
	m_currentSurface = 0;
	m_initialDistanceBetweenFingers=0;
	
	m_move =false;
	m_resize = false;
	m_spinning = false;
	m_path = m_resourceManager->GetResourcePath();
	m_adjustForWideAngles = 0;
	m_preserveOrientation = 1;
	//string path = m_path + GetObjectName(0);
	//ISurface *s = new ObjSurface(path, MAC_FILE);
	ISurface *s = new ObjSurface();
	
	m_surfaces.push_back(s);
	//m_initObject[i] = 1;
	
	m_renderingEngine->InitializeMY(m_surfaces);
    for (int i = 0; i < SurfaceCount; i++) {
		m_objscreenpos[i].x=-1000.0f;
		m_objscreenpos[i].y=-1000.0f;
		m_objscreenpos[i].w=-1000.0f;
		m_objscreenpos[i].z=-1000.0f;
      
	}
	
}

void ApplicationEngine::PopulateVisuals(Visual* visuals) const
{
	
	for (int i = 0; i < SurfaceCount; i++) {
        
		visuals[i].ViewportSize = 
			ivec2((int)m_objscreenpos[i].z, (int)m_objscreenpos[i].w);
		
		visuals[i].LowerLeft = ivec2((int)m_objscreenpos[i].x, (int)m_objscreenpos[i].y);
	
       // visuals[0].Orientation = Quaternion();
		
		//if(i == m_currentSurface) {
		visuals[i].ScreenPos = vec2(m_objscreenpos[i].x, m_objscreenpos[i].y);
		//}
		//else {
		//	visuals[0].ScreenPos = vec2(-1000.0f, -1000.0f);
		//}
		
		if(!m_preserveOrientation) {
			//visuals[i].Orientation = m_orientation[m_currentSurface];
			visuals[i].Orientation = m_orientation[i];
		}
		else {
			visuals[i].Orientation = m_orientation[0];
		}
	}
    
	
}

void ApplicationEngine::Render() 
{
   // vector<Visual> visuals(SurfaceCount);
    Visual visuals[SurfaceCount];
    
	PopulateVisuals(&visuals[0]);
    m_renderingEngine->RenderMY(&visuals[0]);
	
	//for(int i=0; i < SurfaceCount; i++)
	//	m_renderingEngine->RenderMY(visuals, i);
}

void ApplicationEngine::UpdateAnimation(float dt)
{
    if (m_animation.Active) {
        m_animation.Elapsed += dt;
        if (m_animation.Elapsed > m_animation.Duration)
            m_animation.Active = false;
    }
}

void ApplicationEngine::OnFingerUp(vec2 location)
{
   // m_spinning = false;
    m_fingerEnd = location;
    if (m_pressedButton != -1 && m_pressedButton == MapToSurface(location) &&
        !m_animation.Active)
    {
        m_animation.Active = true;
        m_animation.Elapsed = 0;
        m_animation.Duration = 0.25f;
        
	}
    
    m_pressedButton = -1;
}

void ApplicationEngine::OnFingerDown(vec2 location)
{
    m_fingerStart = location;
   
    m_pressedButton = MapToSurface(location);
	//m_pressedButton = 0;
	
	if (m_spinning && (m_pressedButton != -1)) {
		if(m_preserveOrientation) {
			m_previousOrientation[0] = m_orientation[0];
		}
		else {
			m_previousOrientation[m_currentSurface] = m_orientation[m_currentSurface];
		}
	}
}

void ApplicationEngine::OnFingerMove(vec2 oldLocation, vec2 location)
{
	if ((m_spinning) && (m_pressedButton != -1) && (m_pressedButton != MapToSurface(location)))
        m_pressedButton = -1;
	
    if (m_spinning && (m_pressedButton != -1)) {
        vec3 start = MapToSphere(m_fingerStart);
        vec3 end = MapToSphere(location);
        Quaternion delta = Quaternion::CreateFromVectors(start, end);
		if(!m_preserveOrientation) {
			m_orientation[m_currentSurface] = delta.Rotated(m_previousOrientation[m_currentSurface]);
		}
		else {
			m_orientation[0] = delta.Rotated(m_previousOrientation[0]);
		}
		m_currentSurface = m_pressedButton;
    }
	if(m_move && (m_pressedButton != -1)) {
		int currX = 0;
		int currY = 0;
		int flag = 0;
		vec2 viewport = MapToViewport(location, m_pressedButton);
		
 		m_currentSurface = m_pressedButton;
		
		ivec2 size = ivec2(m_objscreenpos[m_pressedButton].z, m_objscreenpos[m_pressedButton].w);
		// See if the object goes out of bounds
		currX = viewport.x + ((size.x)/2);
		currY = viewport.y + ((size.y)/2);
		
		if((currX  > 300) || (currX  < 10)) flag = 1;
		if((currY > 450) || (currY < 60)) flag = 1;
		
		if(!flag) {
		
			m_objscreenpos[m_pressedButton].x = (float)(viewport.x);
			m_objscreenpos[m_pressedButton].y = (float)(viewport.y);
		}
		m_fingerStart = location;
#ifdef DEBUG		
		printf("%6f %6f %6f %6f\n", m_objscreenpos[m_currentSurface].x, m_objscreenpos[m_currentSurface].y,
			   m_fingerStart.x, m_fingerStart.y);
		printf("------");
		printf("\n\n");
#endif
	}
	
	
}

void ApplicationEngine::OnTwoFingerUp(vec2 loc, vec2 loc1)
{
	return;
}

void ApplicationEngine::OnTwoFingerDown(vec2 loc, vec2 loc1)
{
	 m_pressedButton = MapToSurface(loc);
	if(m_resize) {
		
		vec2 touchpoint = loc;
		vec2 touchpoint1 = loc1;
		
		// anyone point should be inside viewport
		touchpoint.y = ((float)(m_screenSize.y)) - touchpoint.y;
		
		for(int surfaceIndex=0; surfaceIndex < SurfaceCount; surfaceIndex++) {
			
			float width = m_objscreenpos[surfaceIndex].z;
			float height = m_objscreenpos[surfaceIndex].w;
			float objWidth=0.0f;
			float objHeight=0.0f;
			float objX = 0.0f;
			float objY = 0.0f;
			
			objX = m_objscreenpos[surfaceIndex].x;
			objY = m_objscreenpos[surfaceIndex].y;
			
			objWidth = m_objscreenpos[surfaceIndex].x + width;
			objHeight = m_objscreenpos[surfaceIndex].y + height;	
			
			if(m_adjustForWideAngles) {
				
				objX -= objWidth;
				//if(objX < 0.0f) objX = 0.0f;
				objY -= objHeight;
				//if(objY < 0.0f) objY = 0.0f;
				
				objWidth *= 2;
				objHeight *= 2;
			}
			
			if((touchpoint.x >= objX) && 
			   (touchpoint.x <= objWidth) &&
			   (touchpoint.y >= objY) && 
			   (touchpoint.y <= objHeight)
			   ) {
				m_pressedButton = surfaceIndex;
				break;
			}
			else if((touchpoint1.x >= objX) && 
				(touchpoint1.x <= objWidth) &&
				(touchpoint1.y >= objY) && 
				(touchpoint1.y <= objHeight)
				) {
				m_pressedButton = surfaceIndex;
					break;
			}
			else {
				m_initialDistanceBetweenFingers = 0;
				return;
			}
		}
		
		float dx = loc.x - loc1.x;
		float dy = loc.y - loc1.y;
		m_initialDistanceBetweenFingers =  sqrtf((dx*dx) + (dy*dy) );
#ifdef DEBUG
		printf("%6f %6f\n", m_scaleFactor, m_initialDistanceBetweenFingers);
		printf("--------\n\n");
#endif
		
		
	}
	return;
}

void ApplicationEngine::OnTwoFingerMove(vec2 currentFinger1, 
										vec2 currentFinger2)
{
	if(!m_initialDistanceBetweenFingers) return;
	if(m_resize) {
		
		float dx = currentFinger2.x - currentFinger1.x;
		float dy = currentFinger2.y - currentFinger1.y;
		float currentDistanceBetween2Fingers =  sqrtf((dx*dx) + (dy*dy) );
#ifdef DEBUG
		printf("%6f %6f\n", m_scaleFactor, m_initialDistanceBetweenFingers);
#endif
		m_scaleFactor = (float)(currentDistanceBetween2Fingers/m_initialDistanceBetweenFingers);
		m_initialDistanceBetweenFingers = currentDistanceBetween2Fingers;
		
		m_objscreenpos[m_pressedButton].z *= m_scaleFactor;
		m_objscreenpos[m_pressedButton].w *= m_scaleFactor;
		
#ifdef DEBUG
		printf("%6f %6f\n", m_scaleFactor, m_initialDistanceBetweenFingers);
		printf("---------------------------------------------------------\n\n");
#endif
	}
}

vec3 ApplicationEngine::MapToSphere(vec2 touchpoint) const
{
    vec2 p = touchpoint - m_centerPoint;
    
    // Flip the Y axis because pixel coords increase towards the bottom.
    p.y = -p.y;
   // p.y = m_screenSize.y - p.y;
	
    const float radius = m_trackballRadius;
    const float safeRadius = radius - 1;
    
    if (p.Length() > safeRadius) {
        float theta = atan2(p.y, p.x);
        p.x = safeRadius * cos(theta);
        p.y = safeRadius * sin(theta);
    }
    
    float z = sqrt(radius * radius - p.LengthSquared());
    vec3 mapped = vec3(p.x, p.y, z);
    return mapped / radius;
}
	
int ApplicationEngine::MapToSurface(vec2 touchpoint) const
{
   	int surfaceIndex=1;
	touchpoint.y = ((float)(m_screenSize.y)) - touchpoint.y;
	
	for(surfaceIndex=0; surfaceIndex < SurfaceCount; surfaceIndex++) {
		float width = m_objscreenpos[surfaceIndex].z;
		float height = m_objscreenpos[surfaceIndex].w;
		
		if((touchpoint.x >= m_objscreenpos[surfaceIndex].x) && 
		   (touchpoint.x <= (m_objscreenpos[surfaceIndex].x + width)) &&
		   (touchpoint.y >= m_objscreenpos[surfaceIndex].y) && 
		   (touchpoint.y <= (m_objscreenpos[surfaceIndex].y + height))
		   ) {
			break;
		}
	}

    if (surfaceIndex >= SurfaceCount)
        return -1;
    
    return surfaceIndex;
}

vec2 ApplicationEngine::MapToViewport(vec2 touchpoint, int surfaceIndex) const
{
	vec2 viewport;
	vec2 delta;
	
	float fingerStart_y =0;
#ifdef DEBUG
	printf("%6f %6f\n", touchpoint.x, touchpoint.y);
#endif	
	fingerStart_y = ((float)(m_screenSize.y)) - m_fingerStart.y;
	
	touchpoint.y = m_screenSize.y - touchpoint.y;
	
	delta.x = touchpoint.x - m_fingerStart.x ;
	delta.y =(float)(touchpoint.y - fingerStart_y );
	
	viewport.y = m_objscreenpos[surfaceIndex].y + delta.y;
	viewport.x = m_objscreenpos[surfaceIndex].x + delta.x;
	
#ifdef DEBUG
	printf("%6f %6f %6f %6f %6f %6f\n", viewport.x, viewport.y, touchpoint.x, touchpoint.y, 
		   m_fingerStart.x, fingerStart_y);
#endif	
	return viewport;
}
	

int ApplicationEngine::SetupScreenPos(int surfaceIndex, float x, float y, float z, float w)
{
	if((surfaceIndex >= SurfaceCount) || (surfaceIndex < 0)) return -1;
	
	//for(int i =0; i < SurfaceCount; i++) {
		
	m_objscreenpos[surfaceIndex].x=x;
	m_objscreenpos[surfaceIndex].y=y;
	m_objscreenpos[surfaceIndex].z=z;
	m_objscreenpos[surfaceIndex].w=w;
		
	//}
	
	return 0;
}
	
int ApplicationEngine::BrowseObjects(int n)
{
	int prevSurface = m_currentSurface;
	switch(n) {
		case FIRST_OBJECT:
			m_currentSurface =0;
			break;
		case PREV_OBJECT:
			if(m_currentSurface > 0) m_currentSurface--;
			else if(m_currentSurface == 0)
				m_currentSurface = (MaxEyeSurfaceCount-1);
			break;
		case NEXT_OBJECT:
			m_currentSurface = (m_currentSurface + 1)%MaxEyeSurfaceCount;
			break;
		case LAST_OBJECT:
			m_currentSurface = MaxEyeSurfaceCount-1;
			break;
		default:
			return -1;
	}
	
	if(prevSurface != m_currentSurface) {

		InitializeObject();
		
	}
	return 0;
}
	

void ApplicationEngine::GetSurfaceScreenPos(int surfaceIndex, float *x, float *y, float *z, float *w)
{
	if(surfaceIndex >= SurfaceCount) return;
	
	*x = m_objscreenpos[surfaceIndex].x;
	*y = m_objscreenpos[surfaceIndex].y;
	*z = m_objscreenpos[surfaceIndex].z;
	*w = m_objscreenpos[surfaceIndex].w;
			
	return;
}
	
bool ApplicationEngine::PresentBuffer() 
{
	return m_renderingEngine->PresentBuffer();
}

void ApplicationEngine::ResetMovement()
{
	m_move = false;
	m_spinning = false;
	m_resize = false;
}

string ApplicationEngine::GetObjectName(int noseIndex) const
{
	string name;
#if 0
	
	switch (noseIndex) {
		case EURO_FEM_NOSE_BRIDGE_DEEP:
			name = string("/euro-fem-nose-bridge-deep.obj");
			break;
		case EURO_FEM_NOSE_BRIDGE_LONG:
			name = string("/euro-fem-nose-bridge-long.obj");
			break;
		case EURO_FEM_NOSE_BRIDGE_SHALLOW:
			name = string("/euro-fem-nose-bridge-shallow.obj");
			break;
		case EURO_FEM_NOSE_BRIDGE_SHORT:
			name = string("/euro-fem-nose-bridge-short.obj");
			break;
		case EURO_FEM_NOSE_DOWN:
			name = string("/euro-fem-nose-down.obj");
			break;
		case EURO_FEM_NOSE_FLAT:
			name = string("/euro-fem-nose-flat.obj");
			break;
		case EURO_FEM_NOSE_LONG:
			name = string("/euro-fem-nose-long.obj");
			break;
		case EURO_FEM_NOSE_POINTED:
			name = string("/euro-fem-nose-pointed.obj");
			break;
		case EURO_FEM_NOSE_REGION_CONCAVE:
			name = string("/euro-fem-nose-region-concave.obj");
			break;
		case EURO_FEM_NOSE_REGION_CONVEX:
			name = string("/euro-fem-nose-region-convex.obj");
			break;
		case EURO_FEM_NOSE_SELLION_DEEP:
			name = string("/euro-fem-nose-sellion-deep.obj");
			break;
		case EURO_FEM_NOSE_SELLION_DOWN:
			name = string("/euro-fem-nose-sellion-down.obj");
			break;
		case EURO_FEM_NOSE_SELLION_SHALLOW:
			name = string("/euro-fem-nose-sellion-shallow.obj");
			break;
		case EURO_FEM_NOSE_SELLION_THIN:
			name = string("/euro-fem-nose-sellion-thin.obj");
			break;
		case EURO_FEM_NOSE_SELLION_UP:
			name = string("/euro-fem-nose-sellion-up.obj");
			break;
		case EURO_FEM_NOSE_SELLION_WIDE:
			name = string("/euro-fem-nose-sellion-wide.obj");
			break;
		case EURO_FEM_NOSE_SHORT:
			name = string("/euro-fem-nose-short.obj");
			break;
		case EURO_FEM_NOSE_TILT_DOWN:
			name = string("/euro-fem-nose-tilt-down.obj");
			break;
		case EURO_FEM_NOSE_TILT_UP:
			name = string("/euro-fem-nose-tilt-up.obj");
			break;
		case EURO_FEM_NOSE_UP:
			name = string("/euro-fem-nose-up.obj");
			break;
		case EURO_FEM_NOSTRIL_TILT_DOWN:
			name = string("/euro-fem-nostril-tilt-down.obj");
			break;
		case EURO_FEM_NOSE_NOSTRIL_TILT_UP:
			name = string("/euro-fem-nostril-tilt-up.obj");
			break;
		case EURO_FEM_NOSTRILS_LARGE:
			name = string("/euro-fem-nostrils-large.obj");
			break;
		case EURO_FEM_NOSTRILS_SMALL:
			name = string("/euro-fem-nostrils-small.obj");
			break;
		case EURO_FEM_NOSTRILS_THIN:
			name = string("/euro-fem-nostrils-thin.obj");
			break;
		case EURO_FEM_NOSTRILS_WIDE:
			name = string("/euro-fem-nostrils-wide.obj");
			break;
		case EURO_NOSE_AVG:
			name = string("/euro-nose-avg.obj");
			break;
		case EURO_NOSE_BRIDGE_DEEP:
			name = string("/euro-nose-bridge-deep.obj");
			break;
		case EURO_NOSE_BRIDGE_LONG:
			name = string("/euro-nose-bridge-long.obj");
			break;
		case EURO_NOSE_BRIDGE_SHALLOW:
			name = string("/euro-nose-bridge-shallow.obj");
			break;
		//case EURO_NOSE_BRIDGE_SHORT1:
		//	name = string("/euro-nose-bridge-short1.obj");
		//	break;
		case EURO_NOSE_BRIDGE_SHORT:
			name = string("/euro-nose-bridge-short.obj");
			break;
		case EURO_NOSE_DOWN:
			name = string("/euro-nose-down.obj");
			break;
		case EURO_NOSE_FEM_AVG:
			name = string("/euro-nose-fem-avg.obj");
			break;
		case EURO_NOSE_FLAT:
			name = string("/euro-nose-flat.obj");
			break;
		case EURO_NOSE_LONG:
			name = string("/euro-nose-long.obj");
			break;
		//case EURO_NOSE_MALE_AVG:
		//	name = string("/euro-nose-male-avg.obj");
		//	break;
		//case EURO_NOSE_NOSTRIL_DOWN:
		//	name = string("/euro-nose-nostril-down.obj");
		//	break;
		case EURO_NOSE_NOSTRIL_THIN:
			name = string("/euro-nose-nostril-thin.obj");
			break;
		case EURO_NOSE_NOSTRIL_UP:
			name = string("/euro-nose-nostril-up.obj");
			break;
		case EURO_NOSE_NOSTRIL_WIDE:
			name = string("/euro-nose-nostril-wide.obj");
			break;
		case EURO_NOSE_POINTED:
			name = string("/euro-nose-pointed.obj");
			break;
		case EURO_NOSE_REGION_CONCAVE:
			name = string("/euro-nose-region-concave.obj");
			break;
		case EURO_NOSE_REGION_CONVEX:
			name = string("/euro-nose-region-convex.obj");
			break;
		case EURO_NOSE_SELLION_DEEP:
			name = string("/euro-nose-sellion-deep.obj");
			break;
		case EURO_NOSE_SELLION_DOWN:
			name = string("/euro-nose-sellion-down.obj");
			break;
		case EURO_NOSE_SELLION_SHALLOW:
			name = string("/euro-nose-sellion-shallow.obj");
			break;
		case EURO_NOSE_SELLION_THIN:
			name = string("/euro-nose-sellion-thin.obj");
			break;
		case EURO_NOSE_SELLION_UP:
			name = string("/euro-nose-sellion-up.obj");
			break;
		case EURO_NOSE_SELLION_WIDE:
			name = string("/euro-nose-sellion-wide.obj");
			break;
		case EURO_NOSE_SHORT:
			name = string("/euro-nose-short.obj");
			break;
		case EURO_NOSE_TILT_DOWN:
			name = string("/euro-nose-tilt-down.obj");
			break;
		case EURO_NOSE_TILT_UP:
			name = string("/euro-nose-tilt-up.obj");
			break;
		case EURO_NOSE_UP:
			name = string("/euro-nose-up.obj");
			break;
		default:
			break;
	}
#endif
	//cout << "OBJ: " << name << endl;
	return name;
}

	
void ApplicationEngine::InitializeObjectOLD()
{
#ifdef USE_ORIG_CODE
	
	m_renderingEngine->DeallocCurrentObject();
    delete m_surfaces[0];
	m_surfaces.clear();
	
	if((m_currentSurface < MaxEyeSurfaceCount) && (m_currentSurface >=0) ) {

		string path = (m_path + GetObjectName(m_currentSurface));
		ISurface *s = new ObjSurface(path, MAC_FILE);
		m_surfaces.push_back(s);
		m_renderingEngine->InitializeBoundingSphere(m_surfaces[0], m_currentSurface);
		
				
	}
#endif
}
	
void ApplicationEngine::InitializeObject()
{
	
	m_renderingEngine->DeallocCurrentObject();
	
	if((m_currentSurface < MaxEyeSurfaceCount) && (m_currentSurface >=0) ) {
		
		m_renderingEngine->SetSurfaceTypeIndex(m_currentSurface);
		m_renderingEngine->InitializeBoundingSphere(m_surfaces[0], m_currentSurface);
		
		
	}
	
}

void ApplicationEngine::PreserveOrientation(int val)
{
	
	m_preserveOrientation=1;
	if(0 == val) m_preserveOrientation=0;
	
}

void ApplicationEngine::SetZoomFactor(float val)
{
	m_scaleFactor = val;
	for(int i=0; i < SurfaceCount; i++) {
		m_objscreenpos[i].z *= m_scaleFactor;
		m_objscreenpos[i].w *= m_scaleFactor;
	}
	return;
}
	
	
} // namespace

