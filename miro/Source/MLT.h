#ifndef CSE168_MLT_H_INCLUDED
#define CSE168_MLT_H_INCLUDED

#include "Utils.h"
#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"
#include "MarkovChain.h"
#include "PathContribution.h"

class MLT
{
public:	
	MLT(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~MLT(void) {};
	float mutate(float value);
    void run();    
	void tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, const MarkovChain& MC, bool log = false) const;
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const;
	Vector3 pathTraceFromPath(std::vector<HitInfo> path) const;
	void accumulatePathContribution(const PathContribution pathContribution, const double scaling) const;
	PathContribution calcPathContribution(const std::vector<HitInfo> path) const;
	Vector3 pathTroughput(const std::vector<HitInfo> path) const;	
	double pathProbabilityDensity(const std::vector<HitInfo> path) const;
	double pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const;
	double MISWeight(const std::vector<HitInfo> path, const int pathLength) const;
	double directionToArea(const HitInfo current, const HitInfo next) const;
	double acceptProb(MarkovChain& current, MarkovChain& proposal) const;
	void calcCoordinates(std::vector<HitInfo> path, int &px, int &py) const;
	std::vector<HitInfo> generateEyePathFromChain(MarkovChain chain) const;
private:
	Scene& scene;
    Image* img;
    Camera* cam;
	int samples;
    MarkovChain MC;
    mutable std::vector<float> picture;
};

#endif // CSE168_MLT_H_INCLUDED
