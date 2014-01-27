#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"
#include "SpecularReflection.h"

;const int DEFAULT_REC_DEPTH = 4;

Vector3 SpecularReflection::shade(const Ray& ray, const HitInfo& hit,
	const Scene& scene, const int recDepth) const {

		Vector3 reflectionColor = Vector3(1.0f);
		HitInfo reflectionHit;
		Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
		Ray rayReflect = Ray(Vector3(hit.P), vReflect);

		if(scene.trace(reflectionHit, rayReflect, 0.001f, 100.0f)) {
			reflectionColor *= reflectionHit.material->shade(rayReflect, reflectionHit, scene, recDepth - 1);
		}
		else {
			//Image based
			//reflectionColor *= getHDRColorFromVector(rayReflect.d);
		}
		return reflectionColor;
}

Vector3 SpecularReflection::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const {
	return shade(ray, hit, scene, DEFAULT_REC_DEPTH);
};
