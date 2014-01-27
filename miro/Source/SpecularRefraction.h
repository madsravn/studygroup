#ifndef CSE168_REFRACTION_H_INCLUDED
#define CSE168_REFRACTION_H_INCLUDED

#include "Material.h"

class SpecularRefraction : public Material
{
public:

	SpecularRefraction() {};
	~SpecularRefraction() {};

	virtual void preCalc() {};
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth) const;
	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
};

#endif // CSE168_REFRACTION_H_INCLUDED