#include "MLT.h"

const int maxRecDepth  = 10; // TODO: Flyt denne konstant, evt. til en klasse med konstanter
const int maxEyeEvents = 10;

// Recursive path tracing
//void MLT::tracePath(Path& path, const Ray &ray, int recDepth, bool log) {
//	
//	if(recDepth >= maxRecDepth)	return;
//	
//	HitInfo hit;
//	Vector3 shadeResult = 0;
//
//    if(log) std::cout << "Ray is " << ray << std::endl;
//	
//	if (!scene.trace(hit, ray, 0.0001f)) return;	// Trace misses scene		
//	
//	path.hits[path.size] = HitInfo(hit);
//	path.size++;
//
//	Ray randomRay = Ray(hit.P, generateRandomRayDirection(hit.N));		// TODO: Reflection and Refraction
//	tracePath(path, randomRay, recDepth + 1, log);
//}
//
//// Trace path from eye
//Path MLT::generateEyePath(Ray& eyeRay) {
//	Path result;
//	result.size = 0;
//	result.add(HitInfo(0.0f, eyeRay.o));
//
//	tracePath(result, eyeRay, 1, maxEyeEvents);
//	return result;
//}
//
//// Random mutation of a path
//float MLT::mutate(float value) {
//	float s1 = 1.0f / 1024, s2 = 1.0f/64;
//	float dv = s2 * exp(-log(s2/s1)*rnd());
//	if (rnd() < 0.5f) {
//		value += dv;
//		if(value > 1)
//			value -= 1;		
//	} else {
//		value -= dv;
//		if (value < 0) 
//			value += 1;
//	}
//	return value;
//}
//
//// Builds an initial path
//Path initialPath() {
//	return Path();
//}
//
//float acceptProb(float x, float y) {
//	// T(y > x) / T(x > y)
//	return 0;
//}
/*
Image* MLT() {
	Path x = initialPath();
	Image * img = 0;
	for (int i = 1; i < 10000; i++) {
		Path y = mutate(x);
		float lum_x = x.getLuminance();
		float lum_y = y.getLuminance();
		float acceptance = acceptProb(lum_x, lum_y);			// T(y < x) / T(x < y)
		if (rnd() < acceptance)
			x = y;
		recordSample(img, x);
	}
	return img;
}
*/