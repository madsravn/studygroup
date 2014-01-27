#ifndef CSE168_KDTREE_H_INCLUDED
#define CSE168_KDTREE_H_INCLUDED
#include <vector>
#include "Object.h"
#include "Ray.h"

class Node
{
public:
    Node();
    ~Node();
    Node* left;
    Node* right;
    Node* parent;
    float xmin,xmax,ymin,ymax,zmin,zmax;
    bool leaf;
    int depth;
    std::vector<Object*> objects;
    void AddObject(Object* obj);
    void Initialize(Object* obj);
    void split(int depth);
    void printDebug();
    void drawBox();
    std::vector<Object*> traversal(const Ray& ray);
    bool rayboxint(const Ray& ray, float bounds[6]);
    float C_b, C_trav;
    void setTrav(int size);
    long traversed;
    int getLeafNodes();
    int getNodes();
    long getTraversed();

};

#endif


