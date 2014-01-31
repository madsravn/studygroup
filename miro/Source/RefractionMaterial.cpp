#include "RefractionMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

static float globalIoR = 1.0f;

Vector3 RefractionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth, bool log) const {
	if (recDepth <= 0) {
		return Vector3(0.0f);
	}

	Vector3 refractionColor = Vector3(1.0f);

	// specular refraction
	bool into = dot(hit.N, -ray.d) > 0;		// Going in?
	float costheta1 = dot(hit.N, -ray.d);

	float my;
	if (into)
		my = globalIoR/ior;
	else
		my = ior/globalIoR;

	float costheta2 = 1 - pow(my, 2) * (1 - pow(costheta1, 2));

    //TODO: 
	if (costheta2 < 0) {
		// Reflect instead
		// return;
	}

	int p = (into ? 1 : -1);
	Vector3 vRefract = my * ray.d - p * (my * costheta1 + sqrt(costheta2)) * hit.N;	// New ray direction
	vRefract.normalize();


	Ray rayRefract = Ray(Vector3(hit.P), vRefract);
	HitInfo refractionHit;
	if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
		refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);
	}
	else {
		//Image based
		refractionColor *= scene.getHDRColorFromVector(rayRefract.d);
	}



	// Taken from Wikipedia.org, taken from An Introduction to Ray Tracing by Andrew S. Glassner
	/*float costheta1 = dot(hit.N, -ray.d);
	float costheta2 = 1 - pow(globalIoR/ior, 2) * (1 - pow(costheta1, 2));
	if (costheta2 >= 0) {
		costheta2 = sqrt(costheta2);
	} else {
		std::cout << "costheta2 sqrt error. Component is negative: " << costheta2 << std::endl;
		costheta2 = 0;
	}

	Vector3 vRefract;
	if (costheta1 >= 0) {	// Going in
		vRefract = (globalIoR/ior) * ray.d + ((globalIoR/ior) * costheta1 - costheta2) * hit.N;
	} else {				// Going out
		vRefract = (ior/globalIoR) * ray.d - ((globalIoR/ior) * costheta1 - costheta2) * hit.N;
	}
	vRefract.normalize();

	Ray rayRefract = Ray(Vector3(hit.P), vRefract);
	HitInfo refractionHit;
	if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
		refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);
	}
	else {
		//Image based
		refractionColor *= scene.getHDRColorFromVector(rayRefract.d);
	}*/

	return refractionColor;
}
