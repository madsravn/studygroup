#include "ReflectionMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

Vector3 ReflectionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth) const {

	if (recDepth <= 0) {
		return Vector3(0,0,0);
	}

	Vector3 reflectionColor = Vector3(1.0f);

	// specular reflection
	HitInfo reflectionHit;
	Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
	Ray rayReflect = Ray(Vector3(hit.P), vReflect);

	if(scene.trace(reflectionHit, rayReflect, 0.001f, 100.0f)) {
		reflectionColor *= reflectionHit.material->shade(rayReflect, reflectionHit, scene, recDepth - 1);
	}
	else {
		//Image based
		reflectionColor *= scene.getHDRColorFromVector(rayReflect.d);
	}

	std::cout << reflectionColor<< std::endl;

	return reflectionColor;
}