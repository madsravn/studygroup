#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "Miro.h"
#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Console.h" 
#include "OpenGL.h"
#include <cmath>
#include "Utils.h"

Camera * g_camera = 0;

static bool firstRayTrace = true; 

const float HalfDegToRad = DegToRad/2.0f;

Camera::Camera() :
    m_bgColor(0,0,0),
    m_renderer(RENDER_OPENGL),
    m_eye(0,0,0),
    m_viewDir(0,0,-1),
    m_up(0,1,0),
    m_lookAt(FLT_MAX, FLT_MAX, FLT_MAX),
    m_fov((45.)*(PI/180.))
{
    calcLookAt();
}


Camera::~Camera()
{

}


void
Camera::click(Scene* pScene, Image* pImage)
{
    calcLookAt();
    static bool firstRayTrace = false;

    if (m_renderer == RENDER_OPENGL)
    {
        glDrawBuffer(GL_BACK);
        pScene->openGL(this);
        firstRayTrace = true;
    }
    else if (m_renderer == RENDER_RAYTRACE)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDrawBuffer(GL_FRONT);
        const clock_t begin_time = clock();
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->raytraceImage(this, g_image);
            firstRayTrace = false;
        }
        
        g_image->draw();
        std::cout << "Rendering time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "seconds" << std::endl;
    }
}


void
Camera::calcLookAt()
{
    // this is true when a "lookat" is not used in the config file
    if (m_lookAt.x != FLT_MAX)
    {
        setLookAt(m_lookAt);
        m_lookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
    }
}


void
Camera::drawGL()
{
    // set up the screen with our camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov(), g_image->width()/(float)g_image->height(),
                   0.01, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 vCenter = eye() + viewDir();
    gluLookAt(eye().x, eye().y, eye().z,
              vCenter.x, vCenter.y, vCenter.z,
              up().x, up().y, up().z);
}

// Project v onto u
Vector3 proj(Vector3 u, Vector3 v) {
    return (dot(v,u)/dot(u,u))*u;
}


Ray
Camera::eyeRay(int x, int y, int imageWidth, int imageHeight)
{
	// compute the camera coordinate system 
	const Vector3 wDir = Vector3(-m_viewDir).normalize(); 
	const Vector3 uDir = cross(m_up, wDir).normalize(); 
	const Vector3 vDir = cross(wDir, uDir);    
    //std::cout << wDir << uDir << vDir << std::endl;

    //std::cout << std::endl << std::endl << "NEW RAY" << std::endl;
	// compute the pixel location in the world coordinate system
	const float aspectRatio = (float)imageWidth / (float)imageHeight; 
	const float imPlaneUPos = -((x + 0.5f) / (float)imageWidth - 0.5f); 
	const float imPlaneVPos = -((y + 0.5f) / (float)imageHeight - 0.5f); 
    //std::cout << "imPlaneUPos = " << imPlaneUPos << " and imPlaneVPos = " << imPlaneVPos << std::endl;
	const Vector3 pixelPos = m_eye + (aspectRatio * FILM_SIZE * imPlaneUPos) * uDir + (FILM_SIZE * imPlaneVPos) * vDir + m_distance * wDir;
    //std::cout << "x = " << x << " og y = " << y << std::endl;

			// set the eye ray
	return Ray(m_eye, (m_eye - pixelPos).normalize());
}

Ray
Camera::randomRay(int imageWidth, int imageHeight, const MarkovChain& MC) {
    // TODO: getNext eller de allerede pertuberede i den størrelse? 
    int x = MC.get(0)*imageWidth;
    int y = MC.get(1)*imageHeight;
    return eyeRay(x,y,imageWidth, imageHeight);
}


float divideBy(Vector3 a, Vector3 b) {
    const float totient = 0.0000001f;
    for(int i = 0; i < 3; ++i) {
        if(b[i] > totient || b[i] < -totient) {
            return a[i]/b[i];
        }
    }
    return 0;
}

void
Camera::rayToPixels(const Ray& ray, int& x, int& y, int imageWidth, int imageHeight) {
    // compute the camera coordinate system 
	const Vector3 wDir = Vector3(-m_viewDir).normalize(); 
	const Vector3 uDir = cross(m_up, wDir).normalize(); 
	const Vector3 vDir = cross(wDir, uDir);    

    // compute the pixel location in the world coordinate system
    const float aspectRatio = (float)imageWidth / (float)imageHeight; 
    Vector3 uDirProjection = proj(uDir, ray.d);
    Vector3 vDirProjection = proj(vDir, ray.d);
    Vector3 wDirProjection = proj(wDir, ray.d);
    // pixelPos's wDir består bare af wDir*m_distance. Så hvis vi finder length(wDir*m_distance) og sammenligner den med wDirProjection
    //std::cout << "wDir*m_distance = " << wDir*m_distance << std::endl;
    //std::cout << "wDirProjection = " << wDirProjection << std::endl;

    int sameDirection = 1;
    if(dot(wDir*m_distance,wDirProjection) < 0) {
        sameDirection = -1;
    }
    float scalar = sameDirection*(wDir*m_distance).length()/wDirProjection.length();
    //std::cout << "scalar = " << scalar << std::endl;
    //std::cout << "wDirProjection*scalar = " << wDirProjection*scalar << std::endl;
    //std::cout << "uDirProjection*scalar = " << uDirProjection*scalar << std::endl;
    //std::cout << "divideBy(uDirProjection*scalar, uDir) = " << divideBy(uDirProjection*scalar, uDir)  << std::endl;
    float imPlaneUPos = divideBy(uDirProjection*scalar, uDir)/(aspectRatio * FILM_SIZE);
    //std::cout << "imPlaneUPos = " << imPlaneUPos << std::endl;
    float imPlaneVPos = divideBy(vDirProjection*scalar, vDir)/(aspectRatio * FILM_SIZE);
    //std::cout << "imPlaneVPos = " << imPlaneVPos << std::endl;
    // TODO: ceil?
    float tx = (0.5f - imPlaneUPos)*imageWidth - 0.5f;
    int itx = std::floor(tx);
    if(tx - itx > 0.5f) {
        x = std::ceil(tx);
    } else {
        x = std::floor(tx);
    }

    float ty = (0.5f - imPlaneVPos)*imageHeight - 0.5f;
    int ity = std::floor(ty);
    if(ty - ity > 0.5f) {
        y = std::ceil(ty);
    } else {
        y = std::floor(ty);
    }
    //std::cout << "x = " << x << " og y = " << y << std::endl;
    //const float imPlaneUPos = -((x + 0.5f) / (float)imageWidth - 0.5f); 
	//const float imPlaneVPos = -((y + 0.5f) / (float)imageHeight - 0.5f); 
}

