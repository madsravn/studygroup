#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"
#include "SpecularRefraction.h"

const int DEFAULT_REC_DEPTH = 4;

Vector3 SpecularRefraction::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, const int recDepth) const {
		Vector3 refractionColor = Vector3(1.0f);
		HitInfo refractionHit;

		// Taken from Wikipedia.org, taken from An Introduction to Ray Tracing by Andrew S. Glassner
		float my1 = 1, my2 = 1.31;
		float costheta1 = dot(hit.N, -ray.d);
		float costheta2 = 1 - pow(my1/my2, 2) * (1 - pow(costheta1, 2));
		if (costheta2 >= 0) {
			costheta2 = sqrt(costheta2);
		} else {
			costheta2 = 0;
			std::cout << "costheta2 sqrt error. Component is negative" << std::endl;
		}

		Vector3 vRefract;
		if (costheta1 >= 0) {	// Going in
			vRefract = (my1/my2) * ray.d + ((my1/my2) * costheta1 - costheta2) * hit.N;
		} else {				// Going out
			vRefract = (my1/my2) * ray.d - ((my1/my2) * costheta1 - costheta2) * hit.N;
		}

		Ray rayRefract = Ray(Vector3(hit.P), vRefract);

		if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
			refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);
		}
		else {
			//Image based
			//refractionColor *= getHDRColorFromVector(rayRefract.d);
		}
		return refractionColor;
}

Vector3 SpecularRefraction::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const {
	return shade(ray, hit, scene, DEFAULT_REC_DEPTH);
};
