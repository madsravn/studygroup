#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "PointLight.h"
#include "BVH.h"

class Camera;
class Image;

class Scene
{
public:
    void addObject(Object* pObj)        {m_objects.push_back(pObj);}
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void preCalc();
    void openGL(Camera *cam);
	Vector3 getHDRColorFromVector( const Vector3 &direction) const;
    void multithread( Ray ray, Camera* cam, Image* img, int i, int j);


    void raytraceImage(Camera *cam, Image *img);
    bool trace(HitInfo& minHit, const Ray& ray,
               float tMin = 0.0f, float tMax = MIRO_TMAX) const;
	Vector3 basicShading(const Ray &ray);
	Vector3 pathTraceShading(const Ray &ray, bool log = false);
	Vector3 tracePath(const Ray &ray, int recDepth, bool log = false);
	Vector3 biPathTraceShading(const Ray &ray);	
protected:

    Objects m_objects;
    BVH m_bvh;
    Lights m_lights;
	Vector3* pfmImage;
};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED
