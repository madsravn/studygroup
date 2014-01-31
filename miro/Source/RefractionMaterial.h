#ifndef CSE168_REFRACTION_H_INCLUDED
#define CSE168_REFRACTION_H_INCLUDED

#include "Material.h"

class RefractionMaterial : public Material {
	public:
		RefractionMaterial(float index = 1.0f) {
			ior = index;
		};

		virtual ~RefractionMaterial() {};

	virtual void preCalc() {};
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth, bool log = false) const;
	void setIoR(float i) {
		ior = i;
	}

	protected:
    float ior; // Index of Refraction
};

#endif
