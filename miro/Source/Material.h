#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"
#include "Ray.h"
#include "MarkovChain.h"
#include <vector>

class Material
{
public:
	//enum materialType {LAMBERT, REFLECT, REFRACT};

	//const materialType type;

    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int recDepth, bool log = false) const;
	
	virtual Vector3 shade(const std::vector<HitInfo>& path, const int pathPosition, const Scene& scene, bool log = false) const;
	
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit,const  MarkovChain& MC) const;
	virtual double getPDF(Vector3 in, Vector3 out, Vector3 normal) const;
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const  MarkovChain& MC) const;
};

#endif // CSE168_MATERIAL_H_INCLUDED
