#include "RefractionMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

static float globalIoR = 1.0f;

Vector3 RefractionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth, bool log) const {
	if (recDepth > 5) {
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

	if (costheta2 <= 0) {
		// Reflect instead
		return Vector3(0.0f);
	}

	int p = (into ? 1 : -1);
	Vector3 vRefract = my * ray.d - p * (my * costheta1 + sqrt(costheta2)) * hit.N;	// New ray direction
	vRefract.normalize();


	Ray rayRefract = Ray(Vector3(hit.P), vRefract);
	HitInfo refractionHit;
	if(scene.trace(refractionHit, rayRefract, 0.001f)) {
		refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth + 1);
	}
	else {
		//Image based
		refractionColor *= scene.getHDRColorFromVector(rayRefract.d);
	}

	return refractionColor;
}
