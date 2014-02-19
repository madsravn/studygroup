#ifndef CSE168_MLT_H_INCLUDED
#define CSE168_MLT_H_INCLUDED

#include "Utils.h"
#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"
#include "MarkovChain.h"

class MLT
{
public:	
	MLT(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~MLT(void) {};
	float mutate(float value);
    void run();    
	void tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, const MarkovChain& MC, bool log = false) const;
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const;
	Vector3 pathTraceFromPath(std::vector<HitInfo> path, Ray &ray) const;
private:
	Scene& scene;
    Image* img;
    Camera* cam;
	int samples;
    MarkovChain MC;
};

#endif // CSE168_MLT_H_INCLUDED
