#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"
#include "MLT.h"

class Material
{
public:
	//enum materialType {LAMBERT, REFLECT, REFRACT};

	//const materialType type;

    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth, bool log = false) const;
	
    /*virtual Vector3 shade(const Path path, const int pathPosition, 
                          const Scene& scene, bool log = false) const;*/
	
	/*virtual Ray bounceRay(const Ray& ray, const HitInfo& hit) const;*/
};

#endif // CSE168_MATERIAL_H_INCLUDED
