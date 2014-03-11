#ifndef CSE168_MLT_H_INCLUDED
#define CSE168_MLT_H_INCLUDED

#include <algorithm>
#include "Utils.h"
#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"
#include "MarkovChain.h"
#include "PathContribution.h"
#include "PathTracer.h"

class PathTracer;

class MLT
{
public:	
	MLT(Scene& scene, Image* image, Camera* camera, int pathSamples, PathTracer* tracer);
	~MLT(void) {};
	float mutate(float value);
    void run();
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const;
	Vector3 pathTraceFromPath(std::vector<HitInfo> path) const;
	void accumulatePathContribution(const PathContribution pathContribution, const double scaling);
	double acceptProb(MarkovChain& current, MarkovChain& proposal) const;
private:
	Scene& scene;
    Image* img;
    Camera* cam;
	int samples;
    MarkovChain MC;
    std::vector<float> picture;
	PathTracer* renderer;
};

#endif // CSE168_MLT_H_INCLUDED
