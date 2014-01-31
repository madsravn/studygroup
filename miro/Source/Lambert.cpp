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

	if (recDepth <= 0) {
		return m_kd;
	}

	Vector3 L = Vector3(0.0f);

	Vector3 diffuseColor = Vector3(0.0f);

	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;

	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); ++lightIter) {

		PointLight* pLight = *lightIter;

		// Check for shadows
		HitInfo lightHit;
		bool lightBlocked = false;

		Vector3 fakeLightPosition = pLight->randomPointonLight(hit.P);
		//fakeLightPosition = pLight->position();
		float realLightDistance = (pLight->position() - hit.P).length();
		Vector3 lightDistance = fakeLightPosition - hit.P;
		Ray lightRay = Ray(hit.P, lightDistance);

		if(scene.trace(lightHit, lightRay, 0.001f)) {
			double llength = lightDistance.length();
			if(lightHit.t < llength && (lightHit.P - hit.P).length() < llength ) {
				std::cout << std::endl;
				std::cout << "hit.P:             \t" << hit.P << std::endl;				
				std::cout << "lightHit.P:        \t" << lightHit.P << std::endl;
				std::cout << "lightHit.t:        \t" << lightHit.t << std::endl;	
				std::cout << "(lightHit.P - hit.P).length():\t" << (lightHit.P - hit.P).length() << std::endl;
				std::cout << "llength:           \t" << llength << std::endl;
				std::cout << "lightDistance:	 \t" << lightDistance << std::endl;
				std::cout << "realLightDistance: \t" << realLightDistance << std::endl;
				lightBlocked = true;
			}
		}

		if(!lightBlocked) {

			// the inverse-squared falloff
			float falloff = lightDistance.length2();

			// normalize the light direction
			lightDistance /= falloff;			// Ændret fra sqrt(falloff)

			// get the diffuse component
			float nDotL = dot(hit.N, lightDistance);

			// Standard diffuse lighting
			//diffuseColor += std::max(0.0f, nDotL/falloff * pLight->wattage() / (4 * PI * PI)) * (pLight->color() * m_kd);
			diffuseColor += m_kd * pLight->color() * std::max(nDotL, 0.0f);

			//std::cout << diffuseColor << std::endl;
	
			// Specular highlight
			Vector3 vHalf = (lightDistance + viewDir)/(lightDistance + viewDir).length();
			diffuseColor += pLight->color() * pow(std::max((dot(vHalf, hit.N)), 0.0f), glossiness);
		}
	}	

	

	L += diffuseColor;

	// add the ambient component
	//L += m_ka;

	return L;
}
