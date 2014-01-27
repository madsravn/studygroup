#include "Material.h"

class HDRMaterial : public Material {
	public:
    HDRMaterial();

	virtual ~HDRMaterial();

	virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth) const;
};

