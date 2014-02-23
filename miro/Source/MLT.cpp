#include "MLT.h"
#include "Constants.h"

const int maxRecDepth  = Constants::MaxPathLength; // TODO: Flyt denne konstant, evt. til en klasse med konstanter


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
	
	Ray randomRay = hit.material->bounceRay(ray, hit, recDepth, MC);		// TODO: Reflection and Refraction
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

void MLT::accumulatePathContribution(const std::vector<HitInfo> path, const double scaling) const {
	
	for (int i = 0; i < path.size(); i++) {
		const HitInfo &hit = path.at(i);
		const int ix = 0, iy = 0;	// skal være pixel position
		const Vector3 color;		// Skal være fladens farve * scaling
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {
			// pixel(x,y) += color
		}
	}
	
	/*
	if (pc.sc == 0) return;
	for (int i = 0; i < pc.n; i++) {
		const int ix = int(pc.c[i].x), iy = int(pc.c[i].y); 
		const Vec c = pc.c[i].c * mScaling;													// Scaled by a scalar
		if ((ix < 0) || (ix >= PixelWidth) || (iy < 0) || (iy >= PixelHeight)) continue;	// Quits if the pixel exists?
		img[ix + iy*PixelWidth] = img[ix + iy*PixelWidth] + c;								// Add color to pixel
	}
	*/
}

PathContribution MLT::calcPathContribution(const std::vector<HitInfo> path) const {
	PathContribution result = PathContribution();

	for (int pathLength = 3; pathLength <= 13; pathLength++) {
		for (int numEyeVertices = 1; numEyeVertices <= std::min(pathLength + 1, (int)path.size()); numEyeVertices++) {

			if (numEyeVertices > path.size()) continue;

			std::vector<HitInfo> subPath = subVector(path, 0, numEyeVertices);
			
			Vector3 direction = (path.at(1).P - path.at(0).P).normalized();

			double px = -1.0f, py = -1.0f;
			// Set px and py based on the direction

			Vector3 throughput = pathTroughput(subPath);
			double probabilityDensity = pathProbabilityDensity(subPath, pathLength);	// Denne bliver også kørt inde i MISWeight, overflødigt
			double weight = MISWeight(subPath, pathLength);
			if (weight <= 0.0f || probabilityDensity <= 0.0f) continue;

			Vector3 color = throughput * (weight / probabilityDensity);
			// Assert color is positive
			if (maxVectorValue(color) <= 0.0f) continue;

			result.colors.push_back(Contribution(px, py, color));
			result.scalarContribution = std::max(maxVectorValue(color), result.scalarContribution);
		}
	}
	return result;
}

Vector3 MLT::pathTroughput(const std::vector<HitInfo> path) const {

}

// Probability density for path with a specific number of vertices
double MLT::pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const {
	double sumPDFs = 0.0f;

	// extended BPT
	double p = 1.0;

	// sampling from the eye
	for (int i = 1; i < numEyeVertices; i++) {		
		if (i == 1) {  // First hit
			p *= 1.0 / double(img->width() * img->height());						// divided by image size
			Vector3 direction = (path.at(i + 1).P - path.at(i).P).normalized();		// Direction from first to second hit
			double cosTheta = dot(direction, cam->viewDir());						// Cosine of angle from camera
			double distanceToScreen = cam->distance() / cosTheta;					// Distance to screen from canvas/lens/whatever
			distanceToScreen = distanceToScreen * distanceToScreen;					// Distance to screen squared
			p /= (cosTheta / distanceToScreen);										// Divided by cosine of angle divided by distance to screen squared				

		} else {																	// Other hits
			// PDF of sampling ith vertex
			Vector3 direction0 = (path.at(i - 1).P - path.at(i).P).normalized();
			Vector3 direction1 = (path.at(i + 1).P - path.at(i).P).normalized();

			p *= path.at(i).material->getPDF(direction0, direction1, path.at(i).N);					
		}
		p *= directionToArea(path.at(i), path.at(i + 1));
	}
	return p;
}

// Probability density for path with all numbers of vertices
double MLT::pathProbabilityDensity(const std::vector<HitInfo> path) const {
	double p = 0.0f;
	for (int numEyeVertices = 0; numEyeVertices <= path.size() + 1; numEyeVertices++) {
		p += pathProbabilityDensity(path, numEyeVertices);
	}
	return p;
}


double MLT::MISWeight(const std::vector<HitInfo> path, const int pathLength) const
{
	int numEyeVertices = path.size();
	const double p_i = pathProbabilityDensity(path, numEyeVertices);
	const double p_all = pathProbabilityDensity(path);

	if(p_i || p_all == 0.0f) {    // Er dette rigtigt? Fancy C shit
		return 0.0f;
	} else {
		return std::max(std::min(p_i / p_all, 1.0), 0.0);
	}
}

double MLT::directionToArea(const HitInfo current, const HitInfo next) const {
	const Vector3 dv = next.P - current.P;					// Distance between vertices
	const double d2 = dot(dv, dv);							// Distance squared
	return abs(dot(next.N, dv)) / (d2 * sqrt(d2));			// dot product of next normal and distance divided by d^3
}

float acceptProb(float x, float y) {
	// T(y > x) / T(x > y)
	return 0;
}
 
