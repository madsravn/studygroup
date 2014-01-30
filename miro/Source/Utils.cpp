#include "Utils.h"
#include "Miro.h"
#include <cmath>
double rnd(void) { 
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123; 
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w; 
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0 / 4294967296.0); 
}

// Based on Toshiyas smallpsmlt
// theory: http://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf  
Vector3 generateRandomRayDirection(Vector3 normal){
	float rand1 = rnd();
	float rand2 = rnd();

	const float temp1 = 2.0 * PI * rand1;
	const float temp2 = pow(rand2, 1.0f / (rand1 + 1.0f));
	const float s = sin(temp1);
	const float c = cos(temp1);
	const float t = sqrt(1.0 - temp2 * temp2);

	Vector3 rayDirection = Vector3(s*t, temp2, c*t);
	
	rayDirection.normalize();
	if(dot(rayDirection, normal) < 0) 
		rayDirection = -rayDirection;

	return rayDirection;
}

