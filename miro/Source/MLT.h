#ifndef CSE168_MLT_H_INCLUDED
#define CSE168_MLT_H_INCLUDED

#include "Utils.h"
#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"

class MLT
{
public:	
	MLT(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {};
	~MLT(void) {};
	float mutate(float value);
    void run();    
	void tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, bool log = false) const;
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay) const;
	Vector3 pathTraceFromPath(std::vector<HitInfo> path, Ray &ray) const;
private:
	Scene& scene;
    Image* img;
    Camera* cam;
	int samples;
};

#endif // CSE168_MLT_H_INCLUDED
