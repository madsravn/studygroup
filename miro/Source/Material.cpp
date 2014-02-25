#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Vector3
Material::shade(const Ray&, const HitInfo&, const Scene&, const int recDepth, bool log) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Material::shade(const std::vector<HitInfo>& path, const int pathPosition, 
						const Scene& scene, bool log) const {
	return Vector3(1.0f, 1.0f, 1.0f);
}

Ray Material::bounceRay(const Ray& ray, const HitInfo& hit,const int recDepth, const MarkovChain& MC) const {
	return Ray();
}

double Material::getPDF(Vector3 in, Vector3 out, Vector3 normal) const
{
	throw std::logic_error("The method or operation is not implemented.");
}
