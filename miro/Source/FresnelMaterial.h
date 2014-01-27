#include "Material.h"
#include "ReflectionMaterial.h"
#include "RefractionMaterial.h"

class FresnelMaterial : public Material {
	public:
		FresnelMaterial(const float index);

		virtual ~FresnelMaterial() {};

	virtual void preCalc() {};
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth) const;

	protected:
    float ior; // Index of Refraction
	RefractionMaterial refractionMaterial;
	ReflectionMaterial reflectionMaterial;
};