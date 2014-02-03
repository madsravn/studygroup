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
	const Vector3 viewDir = -ray.d;
	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); ++lightIter) {

		PointLight* pLight = *lightIter;

		// Check for shadows
		HitInfo lightHit;
		bool lightBlocked = false;

		Vector3 fakeLightPosition = pLight->randomPointonLight(hit.P);
		Vector3 realLightDistance = pLight->position() - hit.P;
		Vector3 lightDistance = fakeLightPosition - hit.P;		
		Ray lightRay = Ray(hit.P, lightDistance);

		if(scene.trace(lightHit, lightRay, 0.001f)) {
			double llength = lightDistance.length();
			if(lightHit.t < llength && (lightHit.P - hit.P).length() < llength ) {
				lightBlocked = true;
			}
		}
		if(!lightBlocked) {

			// Dot of normal and light vector
			float nDotL = std::max(dot(hit.N, realLightDistance), 0.0f);

			// Diffuse color
			Vector3 I_d = m_kd * pLight->color() * 1/(2*M_PI);
			
			// Specular highlight color
			//Vector3 vHalf = (lightDistance + viewDir).normalized();
			//Vector3 I_s = pLight->color() * pow(std::max((dot(vHalf, hit.N)), 0.0f), glossiness);

			L += nDotL * (luminance(m_kd) * I_d)/(1 + luminance(m_kd));
		}
	}

	return L;
}