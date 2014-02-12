#pragma once
#include "utils.h"
#include "Scene.h"

const int MaxEvents = 10000;

struct Path {
	HitInfo hits[MaxEvents];	// Points in path
	int size; 					// Length of path
	Path(){
		size=0;
	}
};

class MLT
{
public:
	MLT(Scene& scene) : scene(scene) { };
	~MLT(void);
	float mutate(float value);											
	void tracePath(Path& path, const Ray &ray, int recDepth, bool log);
	Path generateEyePath(Ray& eyeRay);
private:
	Scene& scene;
};

