#include "MLT.h"

const int maxRecDepth  = 10; // TODO: Flyt denne konstant, evt. til en klasse med konstanter
const int maxEyeEvents = 10;


MLT::MLT(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
    MC.imageWidth = image->width();
    MC.imageHeight = image->height();
}


 //Recursive path tracing
void MLT::tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, const MarkovChain& MC, bool log) const {
	
	if(recDepth >= maxRecDepth)	return;
	
	HitInfo hit;
	Vector3 shadeResult = 0;

    if(log) std::cout << "Ray is " << ray << std::endl;
	
	if (!scene.trace(hit, ray, 0.0001f)) return;	// Trace misses scene		
	
	path.push_back(HitInfo(hit));	
	
	Ray randomRay = hit.material->bounceRay(ray, hit, MC);		// TODO: Reflection and Refraction
    //TODO: FIX!
	if (randomRay.d == Vector3(0.0f)) return;

	tracePath(path, randomRay, recDepth + 1, MC, log);
}

// Trace path from eye
std::vector<HitInfo> MLT::generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const {
	std::vector<HitInfo> result;	
	result.push_back(HitInfo(0.0f, eyeRay.o));

	tracePath(result, eyeRay, 1, MC);
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
    int ai, bi;

    double LargeStepProb = 0.3;

    bool running = true;
    MarkovChain current(img->width(), img->height());
    MarkovChain proposal(img->width(), img->height());
    const int count = 512*512;
    int i = 0;
    while( i < count) {

        double isLargeStepDone;
        if(rnd() <= LargeStepProb) {
            isLargeStepDone = 1.0;
            proposal = current.large_step();
        } else {
            isLargeStepDone = 0.0;
            proposal = current.mutate(img->width(), img->height()); //TODO: Fix
        }
        //InitRandomNumbersByChain(proposal); // Det tror jeg ikke vi behøver - vi sender vores MarkovChain med i stedet.

        //proposal.C = CombinePaths(Gene......); // Det skal vi lige have fikset
        
        double a = 0.5;


        Ray ray = cam->randomRay(img->width(), img->height(), current);

        cam->rayToPixels(ray, ai, bi, img->width(), img->height());

        std::vector<HitInfo> path;

        Vector3 shadeResult = pathTraceFromPath(path, ray);

        img->setPixel(ai,bi, shadeResult);

        i++;

        if(rnd() <= a) {
            current = proposal;
        }

        printf("Rendering Progress: %.3f%%\r", i/float(count)*100.0f);
        fflush(stdout);

    }

    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }


    /*for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			//Ray ray = cam->eyeRay(i, j, img->width(), img->height());				
            Ray ray = cam->randomRay(img->width(), img->height(), MC);

            cam->rayToPixels(ray, a, b, img->width(), img->height());

			
			std::vector<HitInfo> path;
			
			Vector3 shadeResult = pathTraceFromPath(path, ray);
			//img->setPixel(i, j, shadeResult);
            img->setPixel(a, b, shadeResult);
            std::cout << "MC.count = " << MC.count << std::endl;
            MC.reset();

		}
		//img->drawScanline(j);
		//glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}
    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }*/
}

Vector3 MLT::pathTraceFromPath(std::vector<HitInfo> path, Ray &ray) const{
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);

	float inverseSamples = 1.0f / (float)(samples);
	for(int i = 0; i < samples; i++) {
		path = generateEyePath(ray, MC);
		if (path.size() >= 2) {
			shadeResult += path.at(1).material->shade(path, 1, scene) * inverseSamples;
			
		}
	}	
	return shadeResult;
}


float acceptProb(float x, float y) {
	// T(y > x) / T(x > y)
	return 0;
}
