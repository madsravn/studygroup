#ifndef CSE168_REFRACTION_H_INCLUDED
#define CSE168_REFRACTION_H_INCLUDED

#include "Material.h"

class RefractionMaterial : public Material {
public:
	//const type = REFRACT;
	RefractionMaterial(float index = 1.0f) {
		ior = index;
	};

	virtual ~RefractionMaterial() {};

	virtual void preCalc() {};

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, bool log = false) const;

	virtual Vector3 shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log = false) const;

	 	
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const  MarkovChain& MC) const;

	virtual double getPDF(Vector3 in, Vector3 out, Vector3 normal) const;
	
	void setIoR(float i) {
		ior = i;
	};

protected:
	float ior; // Index of Refraction
};

#endif
