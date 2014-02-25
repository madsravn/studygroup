#include "Material.h"

class HDRMaterial : public Material {
	public:
    HDRMaterial();

	virtual ~HDRMaterial();

	virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth) const;

	 	
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const  MarkovChain& MC) const;
};

