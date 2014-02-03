#ifndef UTILS_H_CSE68
#define UTILS_H_CSE68
#include "Vector3.h"
double rnd(void);  
// Based on Toshiyas smallpsmlt
// theory: http://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf  
Vector3 generateRandomRayDirection(Vector3 normal);
Vector3 generateRandomRayDirection();
double maxVectorValue(Vector3 vector);
double luminance(Vector3 color);

#endif
