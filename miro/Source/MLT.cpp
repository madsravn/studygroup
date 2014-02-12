#include "MLT.h"

const int maxRecDepth = 10; // TODO: Flyt denne konstant, evt. til en klasse med konstanter
const int maxEyeEvents = 10;

// Recursive path tracing
void MLT::tracePath(Path& path, const Ray &ray, int recDepth, bool log) {
	
	if(recDepth >= maxRecDepth)	return;
	
	HitInfo hit;
	Vector3 shadeResult = 0;

    if(log) std::cout << "Ray is " << ray << std::endl;
	
	if (!scene.trace(hit, ray, 0.0001f)) return;	// Trace misses scene		
	
	path.hits[path.size] = HitInfo(hit);
	Ray randomRay = Ray(hit.P, generateRandomRayDirection(hit.N));	
	tracePath(path, randomRay, recDepth + 1, log);
}

// Trace path from eye
Path MLT::generateEyePath(Ray& eyeRay) {
	Path result;
	result.size = 0;
	
	Ray ray;					// TODO: This should be something
	tracePath(result, ray, 1, maxEyeEvents);
	return result;
}

// Random mutation of a path
float MLT::mutate(float value) {
	float s1 = 1.0f / 1024, s2 = 1.0f/64;
	float dv = s2 * exp(-log(s2/s1)*rnd());
	if (rnd() < 0.5f) {
		value += dv;
		if(value > 1)
			value -= 1;		
	} else {
		value -= dv;
		if (value < 0) 
			value += 1;
	}
	return value;
}
/*
void MLT() {
	x <- initialPath()
	image <- { array of zeros }
	for i <- 1 to N
		y <- mutate(x)
		a <- acceptProb(x -> y)
		if random() < a
			then x <- y
		recordSample(image, x)
	return image
}
*/