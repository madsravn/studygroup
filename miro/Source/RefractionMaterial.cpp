#include "RefractionMaterial.h"
#include "Scene.h"
#include "PFMLoader.h"

static float globalIoR = 1.0f;

Vector3 RefractionMaterial::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, int recDepth, int maxRecDepth, bool log) const {
	if (recDepth > maxRecDepth && maxRecDepth != 1) {    // Skulle gerne reflektere selvom det er basic shading
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
		//TODO: 
		// Reflect instead
		return Vector3(0.0f);
	}

	int p = (into ? 1 : -1);
	Vector3 vRefract = my * ray.d - p * (my * costheta1 + sqrt(costheta2)) * hit.N;	// New ray direction
	vRefract.normalize();


	Ray rayRefract = Ray(Vector3(hit.P), vRefract);
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
//
Vector3 RefractionMaterial::shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log) const {
	Vector3 shadeResult = Vector3(0.0f);
	if(pathPosition + 1 < path.size()) { // Not last element
		shadeResult = path.at(pathPosition + 1).material->shade(path, pathPosition + 1, scene, log);	
	}
	return shadeResult;
}

Ray RefractionMaterial::bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const MarkovChain& MC) const {

	// specular refraction
	float costheta1 = dot(hit.N, -ray.d);
	bool into = costheta1 > 0;		// Going in?	
	
	float my = into ? globalIoR/ior : ior/globalIoR;
	
	float costheta2 = 1 - pow(my, 2) * (1 - pow(dot(hit.N, ray.d),2));

	if (costheta2 <= 0) {
		//std::cout << "costheta2 <= 0" << std::endl;
		return Ray(hit.P, Vector3(0.0f));
	}

	int p = (into ? 1 : -1);
	
	Vector3 vRefract = (my * ray.d - hit.N * (p * my * dot(hit.N, ray.d) + sqrt(costheta2))).normalized();	// New ray direction
		
	/*
	std::cout << "my * dot(hit.N, ray.d)                                  \t" << my * dot(hit.N, ray.d) << std::endl;
	std::cout << "sqrt(costheta2)                                         \t" << sqrt(costheta2) << std::endl;
	std::cout << "p * (my * dot(hit.N, ray.d) + sqrt(costheta2))          \t" << p * (my * dot(hit.N, ray.d) + sqrt(costheta2)) << std::endl;
	std::cout << "hit.N * (p * (my * dot(hit.N, ray.d) + sqrt(costheta2)))\t" << hit.N * (p * (my * dot(hit.N, ray.d) + sqrt(costheta2))) << std::endl;
	std::cout << "my * ray.d                                              \t" << my * ray.d <<std::endl;
	std::cout << "vRefract                                                \t" << vRefract <<std::endl;
	*/

	Ray rayRefract = Ray(Vector3(hit.P), vRefract);

	return rayRefract;
}

double RefractionMaterial::getPDF(Vector3 in, Vector3 out, Vector3 normal) const {
	return 0.0f;
}
