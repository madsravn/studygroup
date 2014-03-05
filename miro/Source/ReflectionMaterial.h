#ifndef CSE168_REFLECTION_H_INCLUDED
#define CSE168_REFLECTION_H_INCLUDED

#include "Material.h"

class ReflectionMaterial : public Material {
	public:
		//const type = REFLECT;
		virtual void preCalc() {}
		ReflectionMaterial() {}
		~ReflectionMaterial() {}
		virtual Vector3 shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, int maxRecDepth, bool log = false) const;
		virtual Vector3 shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log = false) const;

		 	
		virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const  MarkovChain& MC) const;
		virtual Ray bounceRay(const Ray& ray, const HitInfo& hit) const;
		virtual double getPDF(Vector3 in, Vector3 out, Vector3 normal) const;

};

#endif
