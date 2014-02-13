#ifndef CSE168_MLT_H_INCLUDED
#define CSE168_MLT_H_INCLUDED

#include "Utils.h"
#include "Scene.h"

class MLT
{
public:	
	MLT(Scene& scene) : scene(scene) {};
	~MLT(void) {};
	float mutate(float value);											
	void tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, bool log);
	std::vector<HitInfo> generateEyePath(Ray& eyeRay);
private:
	Scene& scene;
};

#endif // CSE168_MLT_H_INCLUDED
