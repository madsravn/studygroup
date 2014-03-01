#include "Lambert.h"
#include "Ray.h"
#include "PFMLoader.h"
#include "Utils.h"
#include "Constants.h"

#if defined(_WIN32)

double M_PI = 3.14159265358979;
double M_1_PI = 1.0/M_PI;
#endif

bool softShadows = false;

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{
	glossiness = 1000;
	int pfmWidth = 1500, pfmHeight = 1500;
	pfmImage = readPFMImage("hdr/stpeters_probe.pfm", &pfmWidth, &pfmHeight);
}

Vector3 Lambert::getHDRColorFromVector(const Vector3 &direction) const {

	Vector3 ret;
	int pfmWidth = 1500, pfmHeight = 1500;

	float r = (1/PI) * acos(direction.z)/sqrt(pow(direction.x, 2) + pow(direction.y, 2));

	ret = pfmImage[
		(int)(((direction.x * r) + 1) / 2 * pfmWidth) + 
			(int)(((direction.y * r) + 1) / 2 * pfmHeight) * pfmHeight];
		return ret;
}

Lambert::~Lambert()
{
}

Vector3	Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, int maxRecDepth, bool log) const {	
    Vector3 vm_kd(m_kd);
	float m = maxVectorValue((vm_kd));
	if (recDepth > maxRecDepth && maxRecDepth != 1){
		if (rnd() >= m) { 
			return Vector3(0.0f);
        }
    }

	Vector3 illumination_direct = Vector3(0.0f);
	Vector3 illumination_indirect = Vector3(0.0f);

	HitInfo t_hit(hit);
	const Lights *lightlist = scene.lights();

	illumination_direct = calcDirectIllum(t_hit, lightlist, scene, (maxRecDepth == 1));
	
	Ray randomRay = Ray(hit.P, generateRandomRayDirection(hit.N));
	HitInfo randomRayHit;

	// Next ray bounce
	if (maxRecDepth != 1) {
		if(scene.trace(randomRayHit, randomRay, 0.001f)) {
			Vector3 randomRayColor = randomRayHit.material->shade(randomRay, randomRayHit, scene, recDepth + 1, maxRecDepth, log);
			float nDotD = dot(hit.N, randomRay.d);
			illumination_indirect = randomRayColor * nDotD * M_1_PI;
		}
	}

	return m_kd*(illumination_direct + illumination_indirect);
}

Vector3 Lambert::shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log) const {	
	if (pathPosition >= path.size()) {
		return Vector3(0.0f);
	}

    Vector3 vm_kd(m_kd);
	float m = maxVectorValue(vm_kd);
	if (pathPosition > 7 + 1){ 		// Efter 7 bounces
		if (rnd() >= m) {
			return Vector3(0.0f); 						// Hvorfor gør den dette? Skalerer farven til p = 1
		}
	}

	Vector3 illumination_direct = Vector3(0.0f);
	Vector3 illumination_indirect = Vector3(0.0f);

	HitInfo hit = path.at(pathPosition);

	const Lights *lightlist = scene.lights();
	
	illumination_direct = calcDirectIllum(hit, lightlist, scene, false);

	// Next ray bounce
	if (path.size() > pathPosition + 1) {
		HitInfo nextHit = path.at(pathPosition + 1);
		Vector3 nextRayDirection = (nextHit.P - hit.P).normalized();
		Vector3 rayColor = nextHit.material->shade(path, pathPosition + 1, scene, log);	
		float nDotD = dot(hit.N, nextRayDirection);
		illumination_indirect = rayColor * nDotD * M_1_PI;
	}
	
	return m_kd*(illumination_direct + illumination_indirect);
};

Vector3 Lambert::calcDirectIllum(const HitInfo &hit, const Lights *lightlist, const Scene &scene, bool allLights) const {
	Vector3 illumination_direct = Vector3(0.0f);

	// Current point light selected at random
	
	if (allLights) {
		for (int i = 0; i < lightlist->size(); i++) {
			PointLight* pLight = lightlist->at(i);
			illumination_direct += calcLighting(hit, pLight, scene);
		}
	} else {
		PointLight* pLight = lightlist->at(rand() % lightlist->size());
		illumination_direct += calcLighting(hit, pLight, scene);
	}
	return illumination_direct;
}

Vector3 Lambert::calcLighting(const HitInfo &hit, PointLight* pLight, const Scene &scene) const {
	Vector3 illumination_direct = Vector3(0.0f);

	// Light vector
	Vector3 lv;
	if (softShadows){
		lv = (pLight->randomPointonLight(hit.P) - hit.P).normalized();
	} else {
		lv = (pLight->position() - hit.P).normalized();
	}

	// Shadow ray test
	HitInfo lightHit;
	scene.trace(lightHit, Ray(hit.P, lv), 0.001f);		
	bool isLightHit = Vector3(pLight->position() - hit.P).length() <= lightHit.t;
	if (isLightHit)
		illumination_direct += (pLight->wattage() * 
		pLight->color() * 
		std::max(dot(lv, hit.N), 0.0f)) 
		/ pow((pLight->position() - hit.P).length(), 2);

	return illumination_direct;
}

Ray Lambert::bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const MarkovChain& MC) const {
    double rand1 = MC.get(recDepth*Constants::NumRNGsPerEvent);
    double rand2 = MC.get(recDepth*Constants::NumRNGsPerEvent + 1);
	return Ray(hit.P, generateRandomRayDirection(hit.N, rand1, rand2));	
}

Ray Lambert::bounceRay(const Ray& ray, const HitInfo& hit) const {
	return Ray(hit.P, generateRandomRayDirection(hit.N));	
}

double Lambert::getPDF(Vector3 in, Vector3 out, Vector3 normal) const {
	return abs(dot(out, normal)) / PI;
}
