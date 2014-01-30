#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"

//double M_PI = 3.14159265358979;
//double M_1_PI = 1.0/M_PI;

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka)
{
	this->diffuseCoefficient = 1;
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

double rnd(void) { 
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123; 
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w; 
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0 / 4294967296.0); 
}


Lambert::~Lambert()
{
}

Vector3	Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth) const {	

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

		Vector3 l = pLight->position() - hit.P;

		// Check for shadows
		HitInfo lightHit;
		Ray lightRay = Ray(hit.P, l);
		bool lightBlocked = false;
		if(scene.trace(lightHit, lightRay, 0.001f)) {
			double llength = (pLight->position() - hit.P).length();
			if(lightHit.t < llength)
				lightBlocked = true;
		}

		if(!lightBlocked) {

			// the inverse-squared falloff
			float falloff = l.length2();

			// normalize the light direction
			l /= sqrt(falloff);

			// get the diffuse component
			float nDotL = dot(hit.N, l);

			// Standard diffuse lighting
			diffuseColor += std::max(0.0f, nDotL/falloff * pLight->wattage() / (4 * PI * PI)) * (pLight->color() * m_kd);

			// Specular highlight
			Vector3 vHalf = (l + viewDir)/(l + viewDir).length();
			diffuseColor += pLight->color() * pow(std::max((dot(vHalf, hit.N)), 0.0f), glossiness);
			
		}
	}	

	L += diffuseColor*diffuseCoefficient;

	// add the ambient component
	L += m_ka;

	return L;
}
