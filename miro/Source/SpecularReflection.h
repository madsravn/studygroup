#ifndef CSE168_REFLECTION_H_INCLUDED
#define CSE168_REFLECTION_H_INCLUDED

#include "Material.h"

class SpecularReflection : public Material
{
public:

	SpecularReflection() {};
	~SpecularReflection() {};

	virtual void preCalc() {};
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth) const;
	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
};

#endif // CSE168_REFLECTION_H_INCLUDED