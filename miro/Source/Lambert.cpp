#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"
#include "Utils.h"

#if defined(_WIN32)

double M_PI = 3.14159265358979;
double M_1_PI = 1.0/M_PI;
#endif

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{
	this->glossiness = 1000;
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

Vector3	Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, bool log) const {	

	float rr_weight = 1.0f;
	/*if (recDepth > 2) {
		if (random() < rr_probability)
			return Vector3(0.0f);
		else
			rr_weight /= 1 - rr_probability;
	}*/

	Vector3 objectColor = m_kd;

	if (recDepth > 5) {
		
	}

	float m = maxVectorValue(objectColor);
	if (recDepth > 5 || !m) 		// Efter 5 bounces, eller hvis p er nul
		if (rnd() < m) 
			objectColor = objectColor*(1/m); 						// Hvorfor gør den dette? Skalerer farven til p = 1
		else 
			return Vector3(0.0f);

	Vector3 L = Vector3(0.0f);
	const Vector3 viewDir = -ray.d;
	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;

	// Current point light selected at random
	PointLight* pLight = lightlist->at(rand() % lightlist->size());
	// Hit point
	Vector3 p = hit.P;
	// Light color
	Vector3 color_light = pLight->color();
	// Hit point normal
	Vector3 n = hit.N;
	// Light vector
	Vector3 lv = (pLight->position() - p).normalized();
	
	Vector3 illumination_direct;
	Vector3 illumination_indirect;

	HitInfo lightHit;
	if (scene.trace(lightHit, Ray(hit.P, lv), 0.001f) ){
		if (Vector3(pLight->position() - p).length() <= lightHit.t)
			illumination_direct = m_kd * color_light * std::max(dot(lv, n), 0.0f);
	} else { // If it doesn't hit everything, it's shot past the light
		illumination_direct = m_kd * color_light * std::max(dot(lv, n), 0.0f);
	}
	
	Ray randomRay = Ray(p, generateRandomRayDirection(n));
	HitInfo randomRayHit;

	if(scene.trace(randomRayHit, randomRay, 0.001f)) {
		Vector3 randomRayColor = randomRayHit.material->shade(randomRay,randomRayHit, scene, recDepth + 1);
		illumination_indirect = m_kd * randomRayColor * dot(n, randomRay.d) * M_1_PI;
	}

	return illumination_direct + illumination_indirect;
}