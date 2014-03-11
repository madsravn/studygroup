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
Vector3 generateRandomRayDirection(Vector3 normal, float rand1, float rand2){
	Vector3 rayDirection = generateRandomRayDirection(rand1, rand2);

	//onb(rayDirection, normal);
	if(dot(rayDirection, normal) < 0) {
		//std::cout << "dot is negative" << std::endl;
		rayDirection = -rayDirection;
	}

	return rayDirection;
}

Vector3 generateRandomRayDirection(Vector3 normal) {
    return generateRandomRayDirection(normal, rnd(), rnd());
}

Vector3 generateRandomRayDirection(float rand1, float rand2){
	//float rand1 = rnd();
	//float rand2 = rnd();

	const float glossiness = 1.0f;

	const float temp1 = 2.0 * PI * rand1;
	const float temp2 = pow(rand2, 2.0f / (glossiness + 1.0f));
	const float temp3 = pow(rand2, 1.0f / (glossiness + 1.0f));
	const float s = sin(temp1);
	const float c = cos(temp1);
	const float t = sqrt(1.0 - temp2);
	
	Vector3 rayDirection = Vector3(s*t, c*t, temp3);
	
	rayDirection.normalize();	

	return rayDirection;
}

void onb(Vector3 &vector, const Vector3 &n) {
	Vector3 u, w, v = n;
	if (n.z < -0.9999999) {
		u = Vector3( 0.0, -1.0, 0.0); 
		w = Vector3(-1.0, 0.0, 0.0);
	} 
	else {
		const double a = 1.0/(1.0 + n.z); 
		const double b = -n.x * n.y * a;
		u = Vector3(1.0 - n.x * n.x * a, b, -n.x); 
		w = Vector3(b, 1.0 - n.y * n.y * a, -n.y);
	}
	Vector3 vx = Vector3(u.x, v.x, w.x);
	Vector3 vy = Vector3(u.y, v.y, w.y);
	Vector3 vz = Vector3(u.z, v.z, w.z);
	Vector3 tempVector = Vector3(dot(vector, vx), dot(vector, vy), dot(vector, vz));
	vector = tempVector;
}

Vector3 generateRandomRayDirection() {
    return generateRandomRayDirection(rnd(), rnd());
}

double maxVectorValue(const Vector3 &vector) {
	return std::max(vector.x, std::max(vector.y, vector.z));
}

double luminance(Vector3 color) {
	return color.x * 0.2126f + color.y * 0.7152f + color.z * 0.0722f;
}


Vector3 clamp(Vector3 vector, float lowerBound, float upperBound) {
	if (vector.x > upperBound)
		vector.x = upperBound;
	else if (vector.x < lowerBound)
		vector.x = lowerBound;
	if (vector.y > upperBound)
		vector.y = upperBound;
	else if (vector.y < lowerBound)
		vector.y = lowerBound;
	if (vector.z > upperBound)
		vector.z = upperBound;
	else if (vector.z < lowerBound)
		vector.z = lowerBound;
	return vector;
}

double directionToArea(HitInfo current, HitInfo next) {
	const Vector3 dv = next.P - current.P;					// Distance between vertices
	const double d2 = dot(dv, dv);							// Distance squared
	return abs(dot(next.N, dv)) / (d2 * sqrt(d2));			// dot product of next normal and distance divided by d^3
}




