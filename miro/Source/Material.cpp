#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Vector3
Material::shade(const Ray&, const HitInfo&, const Scene&, const int recDepth) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}
