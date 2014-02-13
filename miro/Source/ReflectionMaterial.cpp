#include "ReflectionMaterial.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

Vector3 ReflectionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth, bool log) const {

	if (recDepth > 5) {
		return Vector3(0,0,0);
	}

	// specular reflection
	HitInfo reflectionHit;
	Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
	Ray rayReflect = Ray(Vector3(hit.P), vReflect);

	if(scene.trace(reflectionHit, rayReflect, 0.001f, 100.0f)) {
		return reflectionHit.material->shade(rayReflect, reflectionHit, scene, recDepth + 1);
	}
	else {
		//Image based
		return scene.getHDRColorFromVector(rayReflect.d);
	}
};

Vector3 ReflectionMaterial::shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log) const {
	Vector3 shadeResult = Vector3(0.0f);
	if(pathPosition + 1 < path.size()) { // Not last element
		shadeResult = path.at(pathPosition + 1).material->shade(path, pathPosition + 1, scene, log);	
	}
	return shadeResult;
};

Ray ReflectionMaterial::bounceRay(const Ray& ray, const HitInfo& hit) const {
	Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
	return Ray(Vector3(hit.P), vReflect);
}
