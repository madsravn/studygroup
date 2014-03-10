#pragma once
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "Utils.h"

class PathTracer
{
public:
	PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~PathTracer(void);
	void run();
	Vector3 pathTraceFromPath(std::vector<HitInfo> path) const;
	std::vector<HitInfo> generatePath(const Ray& eyeRay) const;
	PathContribution calcPathContribution(const std::vector<HitInfo> path) const;
	double pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const;
	double MISWeight(const std::vector<HitInfo> path, const int pathLength) const;
	double pathProbabilityDensity(const std::vector<HitInfo> path) const;
private:
	Scene& scene;
	Image* img;
	Camera* cam;
	int samples;
};