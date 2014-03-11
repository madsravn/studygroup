#include "RefractionMaterial.h"
#include "Scene.h"
#include "PFMLoader.h"

static float globalIoR = 1.0f;

Vector3 RefractionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth, int maxRecDepth, bool log) const {
	if (recDepth > maxRecDepth && maxRecDepth != 1) {    // Skulle gerne reflektere selvom det er basic shading
		return Vector3(0.0f);
	}

	Vector3 refractionColor = Vector3(1.0f);

	Ray rayRefract = bounceRay(ray, hit);
	HitInfo refractionHit;
	if(scene.trace(refractionHit, rayRefract, 0.001f)) {
		refractionColor = refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth + 1, maxRecDepth, log);
	}
	else {
		//Image based
		refractionColor = scene.getHDRColorFromVector(rayRefract.d);
	}

	return refractionColor;
}

Vector3 RefractionMaterial::shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log) const {
	Vector3 shadeResult = Vector3(1.0f);
	if(pathPosition + 1 < path.size()) { // Not last element
		shadeResult = path.at(pathPosition + 1).material->shade(path, pathPosition + 1, scene, log);	
	}
	return shadeResult;
}

Ray RefractionMaterial::bounceRay(const Ray& ray, const HitInfo& hit, const MarkovChain& MC) const {	
	return bounceRay(ray, hit);
}

Ray RefractionMaterial::bounceRay(const Ray& ray, const HitInfo& hit) const {

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
		Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
		// Reflect instead
		return Ray(hit.P, vReflect);
	}

	int p = (into ? 1 : -1);
	Vector3 vRefract = my * ray.d - p * hit.N * (my * costheta1 + sqrt(costheta2));	// New ray direction
	vRefract.normalize();

	Ray rayRefract = Ray(Vector3(hit.P), vRefract);

	return rayRefract;
}

double RefractionMaterial::getPDF(Vector3 in, Vector3 out, Vector3 normal) const {
	return 1.0f;
}
