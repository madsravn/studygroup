#include "HDRMaterial.h"
#include "Ray.h"
#include "Scene.h"


HDRMaterial::HDRMaterial() {}
HDRMaterial::~HDRMaterial() {}
    
/*Ray generateRandomRay(Vector3 normal){
	Vector3 rayDirection = Vector3(-normal);

	while (dot(rayDirection,normal) < 0) {
		rayDirection.x = rand();
		rayDirection.y = rand();
		rayDirection.z = rand();
		rayDirection.normalize();
	}

	return Ray();
}*/


Vector3 HDRMaterial::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, const int recDepth) const {

		const int numberOfSamples = 8;

		for (int sample = 0; sample < numberOfSamples; sample++) {
			//Ray randomRay = generateRandomRay(hit.N);
		}
		return 0;
};



