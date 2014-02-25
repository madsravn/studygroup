#include "HDRMaterial.h"
#include "Ray.h"
#include "Scene.h"

HDRMaterial::HDRMaterial() {}
HDRMaterial::~HDRMaterial() {}
    
Vector3 HDRMaterial::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, const int recDepth) const {

		const int numberOfSamples = 8;

		for (int sample = 0; sample < numberOfSamples; sample++) {
			//Ray randomRay = generateRandomRay(hit.N);
		}
		return 0;
};

Ray HDRMaterial::bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const MarkovChain& MC) const {
	return Ray();
}



