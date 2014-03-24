#ifndef CSE168_PATHTRACER_H_INCLUDED
#define CSE168_PATHTRACER_H_INCLUDED

#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "Utils.h"
#include "ITracer.h"

class PathTracer : public ITracer
{
public:
	int renderSeconds = 0;

	PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~PathTracer(void);
	void run();

	Vector3 pathTraceFromPath(std::vector<HitInfo> path) const;
	
	virtual PathContribution calcPathContribution(const MarkovChain& MC) const;
	PathContribution calcPathContribution(const std::vector<HitInfo> path) const;
	
	double pathProbabilityDensity(const std::vector<HitInfo> path) const;
	double pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const;
	
	std::vector<HitInfo> generatePath(const Ray& eyeRay) const;
	std::vector<HitInfo> generatePath(const Ray& eyeRay, const MarkovChain& MC) const;

	double MISWeight(const std::vector<HitInfo> path, const int pathLength) const;	
private:
	Scene& scene;
	Image* img;
	Camera* cam;
	int samples;
	std::vector<float> picture;
};

#endif
