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

Vector3	Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, bool log) const {	

	float rr_weight = 1.0f;

	Vector3 objectColor = m_kd;

	float m = maxVectorValue(objectColor);

    
	if (recDepth > 7){ 		// Efter 7 bounces, eller hvis p er nul
		if (rnd() < m) { 
			objectColor = objectColor*(1/m); 						// Hvorfor g�r den dette? Skalerer farven til p = 1
        } else {
			return Vector3(0.0f);
        }
    }	
	const Lights *lightlist = scene.lights();

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
	
	Vector3 illumination_direct = Vector3(0.0f);
	Vector3 illumination_indirect = Vector3(0.0f);

	// Shadow ray test
	HitInfo lightHit;

    //TODO: Kan vi ramme nogle fejl med 0.0001f?
	scene.trace(lightHit, Ray(hit.P, lv), 0.001f);	
	if (Vector3(pLight->position() - p).length() <= lightHit.t)
	illumination_direct = (color_light * std::max(dot(lv, n), 0.0f)) / pow((pLight->position() - p).length(), 2);
	
	Ray randomRay = Ray(p, generateRandomRayDirection(n));
	HitInfo randomRayHit;

	// Next ray bounce
	if(scene.trace(randomRayHit, randomRay, 0.001f)) {
		Vector3 randomRayColor = randomRayHit.material->shade(randomRay,randomRayHit, scene, recDepth + 1);
		float nDotD = dot(n, randomRay.d);
		illumination_indirect = randomRayColor * nDotD * M_1_PI * 10;
	}

	return m_kd*(illumination_direct + illumination_indirect);
}
