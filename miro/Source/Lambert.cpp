#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include "PFMLoader.h"


Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
    m_kd(kd), m_ka(ka)
{
	this->diffuseCoefficient = 1;
	this->reflectionCoefficient = 0;
	this->refractionCoefficient = 0;
	this->glossiness = 1000;
	int pfmWidth = 1500, pfmHeight = 1500;
	pfmImage = readPFMImage("hdr/stpeters_probe.pfm", &pfmWidth, &pfmHeight);

}
Lambert::Lambert(float diffuseCoefficient, float reflectionCoefficient, float refractionCoefficient, float glossiness, const Vector3 & kd, const Vector3 & ka) :
	m_kd(kd), m_ka(ka) {
		this->diffuseCoefficient = diffuseCoefficient;
		this->reflectionCoefficient = reflectionCoefficient;
		this->refractionCoefficient = refractionCoefficient;
		this->glossiness = glossiness;
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


Vector3 generateRandomRayDirection(Vector3 normal){
	float rand1 = rnd();
	float rand2 = rnd();

	const float temp1 = 2.0 * PI * rand1;
	const float temp2 = pow(rand2, 1.0f / (rand1 + 1.0f));
	const float s = sin(temp1);
	const float c = cos(temp1);
	const float t = sqrt(1.0 - temp2 * temp2);

	Vector3 rayDirection = Vector3(s*t, temp2, c*t);
	
	rayDirection.normalize();
	if(dot(rayDirection, -normal)) 
		rayDirection = -rayDirection;

	return rayDirection;
}

double rnd(void) { 
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123; 
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w; 
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0 / 4294967296.0); 
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth) const
{

	const int numberOfHDRSamples = 128;

    Vector3 L = Vector3(0.0f);

	Vector3 diffuseColor = Vector3(0.0f);
	Vector3 reflectionColor = Vector3(0.0f);
	Vector3 refractionColor = Vector3(0.0f);
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); ++lightIter) {

		PointLight* pLight = *lightIter;
		
		Vector3 l = pLight->position() - hit.P;

		if (diffuseCoefficient > 0) {
			
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
				//std::cout << "Glossiness: " << glossiness << std::endl;
				//std::cout << "pLight->wattage(): " << pLight->wattage() << std::endl;
				
			}
		}
    }

	if (recDepth > 0) {

		// specular reflection
		if(reflectionCoefficient > 0){
			reflectionColor = Vector3(1.0f) - m_kd;
			HitInfo reflectionHit;
			Vector3 vReflect = ray.d - 2.0f * dot(ray.d, hit.N) * hit.N;
			Ray rayReflect = Ray(Vector3(hit.P), vReflect);
					
			if(scene.trace(reflectionHit, rayReflect, 0.001f, 100.0f)) {
				reflectionColor *= reflectionHit.material->shade(rayReflect, reflectionHit, scene, recDepth - 1);
			}
			else {
					//Image based
				reflectionColor *= getHDRColorFromVector(rayReflect.d);
			}

		}

		// specular refraction
		if(refractionCoefficient > 0) {
			refractionColor = Vector3(1.0f) - m_kd;
			HitInfo refractionHit;

			// Taken from Wikipedia.org, taken from An Introduction to Ray Tracing by Andrew S. Glassner
			float my1 = 1, my2 = 1.31;
			float costheta1 = dot(hit.N, viewDir);
			float costheta2 = 1 - pow(my1/my2, 2) * (1 - pow(costheta1, 2));
			if (costheta2 >= 0) {
				costheta2 = sqrt(costheta2);
			} else {
				costheta2 = 0;
				std::cout << "costheta2 sqrt error. Component is negative" << std::endl;
			}
			
			Vector3 vRefract;
			if (costheta1 >= 0) {	// Going in
				float my = my1/my2;
				vRefract = my * ray.d + (my * costheta1 - costheta2) * hit.N;
			} else {				// Going out
				float my = my2/my1;
				vRefract = my * ray.d - (my * costheta1 - costheta2) * hit.N;
			}

			Ray rayRefract = Ray(Vector3(hit.P), vRefract);
				
			if(scene.trace(refractionHit, rayRefract, 0.001f, 100.0f)) {
				refractionColor *= refractionHit.material->shade(rayRefract, refractionHit, scene, recDepth - 1);
			}
			else {
					//Image based

				refractionColor *= getHDRColorFromVector(rayRefract.d);

			}
		}
	}

	L += diffuseColor*diffuseCoefficient;
	L += refractionColor*refractionCoefficient; 
	L += reflectionColor*reflectionCoefficient;
    
    // add the ambient component
    L += m_ka;
	
    return L;
}
