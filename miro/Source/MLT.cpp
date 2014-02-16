#include "MLT.h"

const int maxRecDepth  = 10; // TODO: Flyt denne konstant, evt. til en klasse med konstanter
const int maxEyeEvents = 10;

 //Recursive path tracing
void MLT::tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, bool log) {
	
	if(recDepth >= maxRecDepth)	return;
	
	HitInfo hit;
	Vector3 shadeResult = 0;

    if(log) std::cout << "Ray is " << ray << std::endl;
	
	if (!scene.trace(hit, ray, 0.0001f)) return;	// Trace misses scene		
	
	path.push_back(HitInfo(hit));	
	
	Ray randomRay = hit.material->bounceRay(ray, hit);		// TODO: Reflection and Refraction
	tracePath(path, randomRay, recDepth + 1, log);
}

// Trace path from eye
std::vector<HitInfo> MLT::generateEyePath(Ray& eyeRay) {
	std::vector<HitInfo> result;	
	result.push_back(HitInfo(0.0f, eyeRay.o));

	tracePath(result, eyeRay, 1);
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

// Builds an initial path
std::vector<HitInfo> initialPath() {
	return std::vector<HitInfo>();
}

void MLT::run() {
    
    for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			Ray ray = cam->eyeRay(i, j, img->width(), img->height());				
			
			std::vector<HitInfo> path = generateEyePath(ray);
			
			Vector3 shadeResult = pathTraceFromPath(path);
			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}

}

Vector3 MLT::pathTraceFromPath(std::vector<HitInfo> path) {
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);
	if (path.size() < 2) {
		return shadeResult;
	}
    const int pathSamples = 64;

	float inversePathSamples = 1.0f / (float)(pathSamples);
	for(int i = 0; i < pathSamples; i++) {
		shadeResult += path.at(1).material->shade(path, 1, scene) * inversePathSamples;
	}
	
	return shadeResult;
}


float acceptProb(float x, float y) {
	// T(y > x) / T(x > y)
	return 0;
}

//Image* MLT() {
//	std::vector<HitInfo> x = initialPath();
//	Image * img = 0;
//	for (int i = 1; i < 10000; i++) {
//		std::vector<HitInfo> y = mutate(x);
//		float lum_x = x.getLuminance();
//		float lum_y = y.getLuminance();
//		float acceptance = acceptProb(lum_x, lum_y);			// T(y < x) / T(x < y)
//		if (rnd() < acceptance)
//			x = y;
//		recordSample(img, x);
//	}
//	return img;
//}
