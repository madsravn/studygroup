#ifndef CSE168_REFLECTION_H_INCLUDED
#define CSE168_REFLECTION_H_INCLUDED

#include "Material.h"

class ReflectionMaterial : public Material
{
public:
	virtual void preCalc() {}
    ReflectionMaterial() {}
	~ReflectionMaterial() {}
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth, bool log = false) const;
};

#endif
