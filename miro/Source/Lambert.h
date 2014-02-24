#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"
#include "Scene.h"

class Lambert : public Material
{
public:
    Lambert(const Vector3 & kd = Vector3(1),
		const Vector3 & ka = Vector3(0)) ;
    virtual ~Lambert();

	//const type = LAMBERT;

    const Vector3 & kd() const {return m_kd;}
    const Vector3 & ka() const {return m_ka;}

    void setKd(const Vector3 & kd) {m_kd = kd;}
    void setKa(const Vector3 & ka) {m_ka = ka;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth, bool log = false) const;
	virtual Vector3 shade(const std::vector<HitInfo>& path, const int pathPosition, 
                          const Scene& scene, bool log = false) const;

	Vector3 calcDirectIllum(const HitInfo &hit, const Lights *lightlist, const Scene &scene, Vector3 illumination_direct) const;

	 	
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const int recDepth, const  MarkovChain& MC) const;

	Vector3 getHDRColorFromVector(const Vector3 &direction) const;
	virtual double getPDF(Vector3 in, Vector3 out, Vector3 normal) const;
	float glossiness;

protected:
    Vector3 m_kd;
    Vector3 m_ka;
	Vector3* pfmImage;
};

#endif // CSE168_LAMBERT_H_INCLUDED
