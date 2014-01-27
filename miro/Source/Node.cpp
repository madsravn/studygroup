#include "Node.h"
#include <iostream>
#include "Triangle.h"
#include <algorithm>
#include <cmath>
#include "Miro.h"


Node::Node() {
    xmin = xmax = ymin = ymax = zmin = zmax = 0.0f;
    xmin = ymin = zmin = MIRO_TMAX;
    xmax = ymax = zmax = MIRO_TMIN;
    leaf = false;
    // In order to check if they have been set
    left = NULL;
    right = NULL;
    parent = NULL;
    depth = 0;
    traversed = 0;
}

Node::~Node() {
    if(left != NULL) {
        delete left;
    }
    if(right != NULL) {
        delete right;
    }
}

int
Node::getLeafNodes() {
    if(!leaf) {
        return left->getLeafNodes() + right->getLeafNodes();
    }
    return 1;
}

int
Node::getNodes() {
    if(!leaf) {
        return 1 + left->getNodes() + right->getNodes();
    }
    return 1;
}

long
Node::getTraversed() {
    if(!leaf) {
        return traversed + left->getTraversed() + right->getTraversed();
    }
    return traversed;
}

void
Node::setTrav(int size) {
    // Teapot 577 triangles
    // Bunny20 is 69k*20 = 1.4 mil.
    float m = (45-1)/(14000-5.77);
    float b = 1 - m*5.77;
    std::cout << "size = " << size << " and m = " << m << std::endl;
    C_trav = size/100*m+b;
    C_b = C_trav/2;
    std::cout << "C_b = " << C_b << " and C_trav = " << C_trav << std::endl;
    

}

void
Node::AddObject(Object* obj) {

}

float SAH(int split, std::vector<Object*> objlist, float parentArea); 


void
Node::Initialize(Object* obj) {
    //Triangle* tri = (Triangle*)obj;
    Triangle* tri = const_cast<Triangle*>(reinterpret_cast<Triangle*>(obj));
    if(tri != NULL) {
        std::vector<Vector3> vert = tri->getPoints();
        
        xmin = std::min(std::min(vert.at(0).x,vert.at(1).x),std::min(xmin,vert.at(2).x));
        ymin = std::min(std::min(vert.at(0).y,vert.at(1).y),std::min(ymin,vert.at(2).y));
        zmin = std::min(std::min(vert.at(0).z,vert.at(1).z),std::min(zmin,vert.at(2).z));
        xmax = std::max(std::max(vert.at(0).x,vert.at(1).x),std::max(xmax,vert.at(2).x));
        ymax = std::max(std::max(vert.at(0).y,vert.at(1).y),std::max(ymax,vert.at(2).y));
        zmax = std::max(std::max(vert.at(0).z,vert.at(1).z),std::max(zmax,vert.at(2).z));

        objects.push_back(obj);
    }
}

bool compareX(Object* l, Object* r)  {
    return l->getCenter().x < r->getCenter().x;
}



bool compareY(Object* l, Object* r) {
    return l->getCenter().y < r->getCenter().y;
}


bool compareZ(Object* l, Object* r)  {
    return l->getCenter().z < r->getCenter().z;
}


void
Node::split(int d) {
    depth = d;
    int axis = depth%3; // x == 0, y == 1, z == 2
    
    float min = MIRO_TMAX;
    int minSplit = 1;
    int minAxis = 0;
    int numSplit = 6;
    float parentArea = ((xmax-xmin)*(ymax-ymin)*2 + (zmax-zmin)*(ymax-ymin)*2 + (xmax-xmin)*(zmax-zmin)*2);
    for(int i = 1; i < numSplit+1; i++) {
        for(int a = 0; a < 3; a++) {
            if(a == 0) {
                std::sort(objects.begin(), objects.end(), compareX);
            } else if(a == 1) {
                std::sort(objects.begin(), objects.end(), compareY);
            } else if(a == 2) {
                std::sort(objects.begin(), objects.end(), compareZ);
            }
            float min_candidate = SAH((objects.size()/numSplit)*i, objects, parentArea);
            if( min_candidate < min) {
                min = min_candidate;
                minAxis = a;
                minSplit = i;
            }
        }
    }
        
    // Now we found the minimum split
    // Let's check if that is good enough for us to split
    float C_nonsplit = objects.size();
    // min contains both right and left splits
    //C_b = 1.5;
    //C_trav = 3.0;
    // With teapot C_b and C_trav should be lower (1.5,3.0);
    float C_split = min + 2*C_b + C_trav;
    //std::cout << "level: " << depth << " C_nonsplit = " << C_nonsplit << " C_split = " << C_split << " and C_trav = " << C_trav << " and C_b " << C_b << std::endl;
    if(C_nonsplit < C_split) {
    //if(depth == 6) {
        // stop subdividing
        leaf = true;
    } else {
        if(minAxis == 0) {
            std::sort(objects.begin(), objects.end(), compareX);
        } else if(minAxis == 1) {
            std::sort(objects.begin(), objects.end(), compareY);
        } else if(minAxis == 2) {
            std::sort(objects.begin(), objects.end(), compareZ);
        }
        left = new Node();
        right = new Node();
        left->parent = this;
        right->parent = this;
        left->depth=d+1;
        right->depth=d+1;
    
        size_t half = (objects.size()/numSplit)*minSplit;
        //size_t half = objects.size()/2;
        for(size_t i = 0; i < half; i++) {
            left->Initialize(objects.at(i));
        }
        for(size_t i = half; i < objects.size(); i++) {
            right->Initialize(objects.at(i));
        }
        left->C_b = C_b;
        left->C_trav = C_trav;
        right->C_b = C_b;
        right->C_trav = C_trav;
        left->split(depth+1);
        right->split(depth+1);
    }

}

//TODO: Remember to sort the list according to axis before calling
//This will save you one sort
float SAH(int split, std::vector<Object*> objlist, float parentArea) {
    float xmin,xmax,ymin,ymax,zmin,zmax;
    xmin = xmax = ymin = ymax = zmin = zmax = 0.0f;
    xmin = zmin = ymin = MIRO_TMAX;
    xmax = ymax = zmax = MIRO_TMIN;
    float result = 0.0f;

    for(size_t i = 0; i < split && i < objlist.size(); i++) {
        Triangle* tri = const_cast<Triangle*>(reinterpret_cast<Triangle*>(objlist.at(i)));
        if(tri != NULL) {
            std::vector<Vector3> vert = tri->getPoints();

            xmin = std::min(std::min(vert.at(0).x,vert.at(1).x),std::min(xmin,vert.at(2).x));
            ymin = std::min(std::min(vert.at(0).y,vert.at(1).y),std::min(ymin,vert.at(2).y));
            zmin = std::min(std::min(vert.at(0).z,vert.at(1).z),std::min(zmin,vert.at(2).z));
            xmax = std::max(std::max(vert.at(0).x,vert.at(1).x),std::max(xmax,vert.at(2).x));
            ymax = std::max(std::max(vert.at(0).y,vert.at(1).y),std::max(ymax,vert.at(2).y));
            zmax = std::max(std::max(vert.at(0).z,vert.at(1).z),std::max(zmax,vert.at(2).z));


        }
                
    }
    result = ((xmax-xmin)*(ymax-ymin)*2 + (zmax-zmin)*(ymax-ymin)*2 + (xmax-xmin)*(zmax-zmin)*2)*split/parentArea;
    xmin = ymin = zmin = MIRO_TMAX;
    xmax = ymax = zmax = MIRO_TMIN;
    for(size_t i = split; i < objlist.size();i++) {
        Triangle* tri = const_cast<Triangle*>(reinterpret_cast<Triangle*>(objlist.at(i)));
        if(tri != NULL) {
            std::vector<Vector3> vert = tri->getPoints();
            xmin = std::min(std::min(vert.at(0).x,vert.at(1).x),std::min(xmin,vert.at(2).x));
            ymin = std::min(std::min(vert.at(0).y,vert.at(1).y),std::min(ymin,vert.at(2).y));
            zmin = std::min(std::min(vert.at(0).z,vert.at(1).z),std::min(zmin,vert.at(2).z));
            xmax = std::max(std::max(vert.at(0).x,vert.at(1).x),std::max(xmax,vert.at(2).x));
            ymax = std::max(std::max(vert.at(0).y,vert.at(1).y),std::max(ymax,vert.at(2).y));
            zmax = std::max(std::max(vert.at(0).z,vert.at(1).z),std::max(zmax,vert.at(2).z));


        }

    }
    result += ((xmax-xmin)*(ymax-ymin)*2 + (zmax-zmin)*(ymax-ymin)*2 + (xmax-xmin)*(zmax-zmin)*2)*(objlist.size()-split)/parentArea;
    return result;
}



std::vector<Object*> 
Node::traversal(const Ray& ray) {
    std::vector<Object*> objs;
    // Check if the ray hits the box
    float bounds[6] = {xmin,ymin,zmin,xmax,ymax,zmax};
    bool hitBox = rayboxint(ray,bounds);
    if(hitBox && objects.size()>0) {
        //traversed++; // For statistics
        if(leaf==true) {
            //for(size_t i = 0; i < objects.size(); i++) {
            //    objs.push_back(objects.at(i));
            //}
            objs.insert(objs.end(), objects.begin(), objects.end());
        } else {
            std::vector<Object*> temp = right->traversal(ray);
            objs.insert(objs.end(), temp.begin(), temp.end());
            temp = left->traversal(ray);
            objs.insert(objs.end(), temp.begin(), temp.end());
        }
    }

    // Will return empty if ray doesn't hit
    // or there are no objects in this branch
    return objs;
}

//Source: http://tog.acm.org/resources/GraphicsGems/ (Graphics Gems I)
bool
Node::rayboxint(const Ray& ray, float bounds[6] )  {

    const int NUMDIM = 3;
    const int RIGHT = 0;
    const int LEFT	= 1;
    const int MIDDLE = 2;

    //char HitBoundingBox(minB,maxB, origin, dir,coord)
    double minB[NUMDIM], maxB[NUMDIM];		/*box */
    minB[0] = bounds[0];
    minB[1] = bounds[1];
    minB[2] = bounds[2];
    maxB[0] = bounds[3];
    maxB[1] = bounds[4];
    maxB[2] = bounds[5];
    double origin[NUMDIM], dir[NUMDIM];		/*ray */
    origin[0] = ray.o.x;
    origin[1] = ray.o.y;
    origin[2] = ray.o.z;
    dir[0] = ray.d.x;
    dir[1] = ray.d.y;
    dir[2] = ray.d.z;
    double coord[NUMDIM];				/* hit point */

	char inside = true;
	char quadrant[NUMDIM];
	register int i;
	int whichPlane;
	double maxT[NUMDIM];
	double candidatePlane[NUMDIM];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<NUMDIM; i++)
		if(origin[i] < minB[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			inside = false;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			inside = false;
		}else	{
			quadrant[i] = MIDDLE;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		coord[0] = origin[0];coord[1] = origin[1];coord[2]=origin[2];
		return true;
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return false;
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return false;
		} else {
			coord[i] = candidatePlane[i];
		}
	return true;				/* ray hits box */
}	


void
Node::printDebug() {
    std::cout << "Depth = " << depth << std::endl;
    std::cout << "Size of the box:" << std::endl;
    std::cout << "(" << xmin << ", " << ymin << ", " << zmin << ")";
    std::cout << " to (" << xmax << ", " << ymax << ", " << zmax << ")." << std::endl;
    std::cout << "Containing " << objects.size() << " elements." << std::endl;
}
       
void Node::drawBox() {
    if(!leaf) {
        left->drawBox();
        right->drawBox();
    } else {
        if(objects.size() != 0) {
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_QUADS);
            glVertex3f(xmin,ymin,zmax);
            glVertex3f(xmax,ymin,zmax);
            glVertex3f(xmax,ymax,zmax);
            glVertex3f(xmin,ymax,zmax);
            
            glVertex3f(xmin,ymin,zmin);
            glVertex3f(xmax,ymin,zmin);
            glVertex3f(xmax,ymax,zmin);
            glVertex3f(xmin,ymax,zmin);

            glVertex3f(xmin,ymin,zmin);
            glVertex3f(xmax,ymin,zmin);
            glVertex3f(xmax,ymin,zmax);
            glVertex3d(xmin,ymin,zmax);
            
            glVertex3f(xmin,ymax,zmin);
            glVertex3f(xmax,ymax,zmin);
            glVertex3f(xmax,ymax,zmax);
            glVertex3d(xmin,ymax,zmax);

            glVertex3f(xmin,ymin,zmin);
            glVertex3f(xmin,ymin,zmax);
            glVertex3f(xmin,ymax,zmax);
            glVertex3f(xmin,ymax,zmin);
            
            glVertex3f(xmax,ymin,zmin);
            glVertex3f(xmax,ymin,zmax);
            glVertex3f(xmax,ymax,zmax);
            glVertex3f(xmax,ymax,zmin);

        glEnd();
        glColor3f(1.0,1.0,1.0);
        }
    }
}
