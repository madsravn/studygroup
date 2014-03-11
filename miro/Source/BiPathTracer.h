#pragma once
#include "Utils.h"
#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"
#include "MarkovChain.h"
#include "PathContribution.h"

class BiPathTracer
{
public:
	BiPathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~BiPathTracer(void);
	void run();
	Vector3 pathTraceFromPath(std::vector<HitInfo> path) const;
	void accumulatePathContribution(const PathContribution pathContribution, const double scaling) const;
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay) const;
	std::vector<HitInfo> generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const;
	std::vector<HitInfo> generateLightPath(const Vector3 lightPos) const;
	std::vector<HitInfo> generateLightPath(const Vector3 lightPos, const MarkovChain& MC) const;
	bool isConnectable(const std::vector<HitInfo> eyePath, const std::vector<HitInfo> lightPath) const;
	PathContribution calcCombinePaths(const std::vector<HitInfo> eyePath, const std::vector<HitInfo> lightPath) const;

	PathContribution calcPathContribution(const MarkovChain& MC) const;
private:
	Scene& scene;
	Image* img;
	Camera* cam;
	int samples;
	mutable std::vector<float> picture;
};

