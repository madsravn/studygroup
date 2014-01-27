#include <ctime>
#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "Node.h"

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
    node = new Node(); 
    for(size_t i = 0; i < m_objects->size(); ++i) {
        node->Initialize((*m_objects)[i]);
    }
    
    node->setTrav(m_objects->size());
    
    const clock_t begin_time = clock();
    
    node->split(1);
    
    
    std::cout << "Building time: " << float( clock() - begin_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << "leaf nodes: " << node->getLeafNodes() << " and nodes: " << node->getNodes() << std::endl;
    
}

BVH::~BVH() {
    std::cout << "Destructor was called" << std::endl;
}

void
BVH::drawBoxes() {
    node->drawBox();
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.
    // ORIGINAL
    /*bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }
    return hit;*/
    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    std::vector<Object*> nodes = node->traversal(ray); 
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        if (nodes.at(i)->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }

    return hit;

}
