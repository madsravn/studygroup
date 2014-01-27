#include "RefractionMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

Vector3 RefractionMaterial::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, int recDepth) const {
		
	Vector3 refractionColor = Vector3(0.0f);
	if (recDepth > 0) {

		// specular refraction
			refractionColor = Vector3(1.0f);
			HitInfo refractionHit;

			// Taken from Wikipedia.org, taken from An Introduction to Ray Tracing by Andrew S. Glassner
			float costheta1 = dot(hit.N, -ray.d);
			float costheta2 = 1 - pow(scene.globalIoR/ior, 2) * (1 - pow(costheta1, 2));
			if (costheta2 >= 0) {
				costheta2 = sqrt(costheta2);
			} else {
				std::cout << "costheta2 sqrt error. Component is negative: " << costheta2 << std::endl;
				costheta2 = 0;
			}

			Vector3 vRefract;
			if (costheta1 >= 0) {	// Going in
				vRefract = (scene.globalIoR/ior) * ray.d + ((scene.globalIoR/ior) * costheta1 - costheta2) * hit.N;
			} else {				// Going out
				vRefract = (ior/scene.globalIoR) * ray.d - ((scene.globalIoR/ior) * costheta1 - costheta2) * hit.N;
			}
			vRefract.normalize();

			Ray rayRefract = Ray(Vector3(hit.P), vRefract);
				
			if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
				refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);
			}
			else {
					//Image based
				refractionColor *= scene.getHDRColorFromVector(rayRefract.d);
			}
		}

	return refractionColor;
}