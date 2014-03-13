#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"
#include "Scene.h"



class Lambert : public Material
{
public:
    Lambert(const Vector3 & kd = Vector3(1),
		const Vector3 & ka = Vector3(0), 
		const Vector3 & ke = Vector3(0));
    virtual ~Lambert();

	//const type = LAMBERT;

    const Vector3 & kd() const {return m_kd;}
    const Vector3 & ka() const {return m_ka;}

    void setKd(const Vector3 & kd) {m_kd = kd;}
    void setKa(const Vector3 & ka) {m_ka = ka;}
	void setKe(const Vector3 & ke) { m_ke = ke; }

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int recDepth, int maxRecDepth, bool log = false) const;
	virtual Vector3 shade(const std::vector<HitInfo>& path, const int pathPosition, 
                          const Scene& scene, bool log = false) const;

	Vector3 calcDirectIllum(const HitInfo &hit, const Lights *lightlist, const Scene &scene, bool allLights) const;
	Vector3 calcLighting(const HitInfo &hit, PointLight* pLight, const Scene &scene) const;

	 	
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit, const  MarkovChain& MC) const;
	virtual Ray bounceRay(const Ray& ray, const HitInfo& hit) const;

	virtual double getPDF(Vector3 in, Vector3 out, Vector3 normal) const;
	float glossiness;

	

protected:
    Vector3 m_kd;
    Vector3 m_ka;
	Vector3 m_ke;
	Vector3* pfmImage;
};

#endif // CSE168_LAMBERT_H_INCLUDED
