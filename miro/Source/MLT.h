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
	MLT(Scene& scene, Image* image, Camera* camera) : scene(scene), img(image), cam(camera) {};
	~MLT(void) {};
	float mutate(float value);											
	void tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, bool log = false);
	std::vector<HitInfo> generateEyePath(Ray& eyeRay);
    void run();
    Vector3 pathTraceFromPath(std::vector<HitInfo> path);
private:
	Scene& scene;
    Image* img;
    Camera* cam;
};

#endif // CSE168_MLT_H_INCLUDED
