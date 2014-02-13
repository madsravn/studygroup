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

/*
Vector3 Material::shade(const std::vector<HitInfo>& path, const int pathPosition, 
						const Scene& scene, bool log = false) const {
	
}*/
	/*
Ray Material::bounceRay(const Ray& ray, const HitInfo& hit) const {
	return Ray();
}*/