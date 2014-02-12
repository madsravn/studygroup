#ifndef CSE168_REFLECTION_H_INCLUDED
#define CSE168_REFLECTION_H_INCLUDED

#include "Material.h"

class ReflectionMaterial : public Material {
	public:
		//const type = REFLECT;
		virtual void preCalc() {}
		ReflectionMaterial() {}
		~ReflectionMaterial() {}
		virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
							  const Scene& scene, const int recDepth, bool log = false) const;
		virtual Vector3 shade(const Path path, const int pathPosition, 
							  const Scene& scene, bool log = false) const;
		virtual Ray bounceRay(const Ray& ray, const HitInfo& hit) const;
};

#endif
