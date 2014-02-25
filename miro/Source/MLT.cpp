#include "MLT.h"
#include "Constants.h"

const int maxRecDepth  = Constants::MaxPathLength; // TODO: Flyt denne konstant, evt. til en klasse med konstanter


MLT::MLT(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
    MC.imageWidth = image->width();
    MC.imageHeight = image->height();
    for(int i = 0; i < img->height()*img->width(); ++i) {
        picture.push_back(Vector3(0.0f));
    }
}


 //Recursive path tracing
void MLT::tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, const MarkovChain& MC, bool log) const {
	//std::cout << "tracePath" << std::endl;
	
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
	//std::cout << "generateEyePath" << std::endl;
	std::vector<HitInfo> result;	
	result.push_back(HitInfo(0.0f, eyeRay.o));

	tracePath(result, eyeRay, 1, MC);
	return result;
}

// Random mutation of a path
float MLT::mutate(float value) {
	//std::cout << "mutate" << std::endl;
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
	std::cout << "initialPath" << std::endl;
	return std::vector<HitInfo>();
}

void MLT::run() {    

	/*	
	function metropolisLightTransport()
		x <- initialPath()
		image <- { array of zeros }
		for i <- 1 to N
			y <- mutate(x)
			a <- acceptProb(x -> y
			if random < a
				then x <- y
			recordSample(image, x)
		return image	
	*/
    int ai, bi;

    double LargeStepProb = 0.3;

	double b = 0.0f;
	// TODO: Estimate normalization constant

    bool running = true;
    MarkovChain current(img->width(), img->height());
    MarkovChain proposal(img->width(), img->height());
    const int count = 512*512;
    int i = 0;

    // Paint over the geometry scene
    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }
    while( i < count) {
		//std::cout << i << std::endl;

        double isLargeStepDone;
        if(rnd() <= LargeStepProb) {
            isLargeStepDone = 1.0;
            proposal = current.large_step();
        } else {
            isLargeStepDone = 0.0;
            proposal = current.mutate(img->width(), img->height()); //TODO: Fix
        }
        //InitRandomNumbersByChain(proposal); // Det tror jeg ikke vi behøver - vi sender vores MarkovChain med i stedet.
        
        
        double a = 0.5;

        Ray ray = cam->randomRay(img->width(), img->height(), current);
        cam->rayToPixels(ray, ai, bi, img->width(), img->height());

        std::vector<HitInfo> path = generateEyePath(ray, MC); // TODO: Skal den her evt. være et eller andet

		proposal.contribution = calcPathContribution(path);

        Vector3 shadeResult = pathTraceFromPath(path);

        img->setPixel(ai, bi, shadeResult);
        img->drawPixel(ai,bi);
        glFinish();

        i++;

		// TODO: accumulate samples
		if (proposal.contribution.scalarContribution > 0.0f)
			accumulatePathContribution(proposal.contribution, (a + isLargeStepDone)/(proposal.contribution.scalarContribution/b + isLargeStepDone));
		if (current.contribution.scalarContribution > 0.0f)
			accumulatePathContribution(current.contribution, (1.0 - a)/(current.contribution.scalarContribution/b + isLargeStepDone));

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

Vector3 MLT::pathTraceFromPath(std::vector<HitInfo> path) const{
	//std::cout << "pathTraceFromPath" << std::endl;
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);
	
	if (path.size() >= 2) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}
		
	return shadeResult;
}

// TODO: Ikke færdig
void MLT::accumulatePathContribution(const PathContribution pathContribution, const double scaling) const {	
	//std::cout << "accumulatePathContribution" << std::endl;
	for (int i = 0; i < pathContribution.colors.size(); i++) {
		const int ix = int(pathContribution.colors.at(i).x);
		const int iy = int(pathContribution.colors.at(i).y);
		//const Vector3 color = pathContribution.colors * scaling;		// TODO: Skal være fladens farve * scaling
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {		
			//img->setPixel(img->getPixel(x, y) + color);				// TODO: Implementer getPixel()
            Vector3 color = Vector3(0.0f);
            color = color + picture[iy*img->width() + ix];
            (picture.at(iy*img->width() + ix)).set(color);
		}
	}
}

PathContribution MLT::calcPathContribution(const std::vector<HitInfo> path) const {
	//std::cout << "calcPathContribution" << std::endl;
	PathContribution result = PathContribution();

	for (int pathLength = 3; pathLength <= 13; pathLength++) {
		for (int numEyeVertices = 1; numEyeVertices <= std::min(pathLength + 1, (int)path.size()); numEyeVertices++) {

			if (numEyeVertices > path.size()) continue;
			if (pathLength > numEyeVertices) continue;

			std::vector<HitInfo> subPath = subVector(path, 0, numEyeVertices);
			
			Vector3 direction = (path.at(1).P - path.at(0).P).normalized();

			double px = -1.0f, py = -1.0f;
			// TODO: Set px and py based on the direction

			Vector3 throughput = pathTraceFromPath(path); //pathTroughput(subPath);

			double probabilityDensity = pathProbabilityDensity(subPath, pathLength);	// Denne bliver også kørt inde i MISWeight, overflødigt? Nææh
			if (probabilityDensity <= 0.0f) continue;

			double weight = MISWeight(subPath, pathLength);
			if (weight <= 0.0f) continue;

			Vector3 color = throughput * (weight / probabilityDensity);

			// Assert color is positive
			if (maxVectorValue(color) <= 0.0f) continue;

			result.colors.push_back(Contribution(px, py, color));
			result.scalarContribution = std::max(maxVectorValue(color), result.scalarContribution);
		}
	}
	return result;
}

// Jeg antager at dette er shade-funktionen
Vector3 MLT::pathTroughput(const std::vector<HitInfo> path) const {
	return pathTraceFromPath(path);
}

// Probability density for path with a specific number of vertices
double MLT::pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const {	
	//std::cout << "pathProbabilityDensity(numEyeVertices = " << numEyeVertices << ")" << std::endl;

	double p = 1.0;

	// sampling from the eye
	for (int i = 1; i < numEyeVertices - 1; i++) {		
		if (i == 1) {  // First hit
			p *= 1.0 / double(img->width() * img->height());						// divided by image size
			Vector3 direction = (path.at(i + 1).P - path.at(i).P).normalized();		// Direction from first to second hit
			double cosTheta = dot(direction, cam->viewDir());						// Cosine of angle from camera
			double distanceToScreen = cam->getDistance() / cosTheta;				// Distance to screen from canvas/lens/whatever
			distanceToScreen = distanceToScreen * distanceToScreen;					// Distance to screen squared
			p /= (cosTheta / distanceToScreen);										// Divided by cosine of angle divided by distance to screen squared				

		} else {																	// Other hits
			// PDF of sampling ith vertex
			Vector3 directionIn = (path.at(i - 1).P - path.at(i).P).normalized();
			Vector3 directionOut = (path.at(i + 1).P - path.at(i).P).normalized();

			p *= path.at(i).material->getPDF(directionIn, directionOut, path.at(i).N);					
		}
		p *= directionToArea(path.at(i), path.at(i + 1));
	}
	return p;
}

// Probability density for path with all numbers of vertices
double MLT::pathProbabilityDensity(const std::vector<HitInfo> path) const {
	//std::cout << "pathProbabilityDensity" << std::endl;
	double p = 0.0f;
	for (int numEyeVertices = 0; numEyeVertices <= path.size(); numEyeVertices++) {
		p += pathProbabilityDensity(path, numEyeVertices);										//Hvis vi skal bruge TKhanAdder ligesom Toshiya skal den tilføjes her
	}
	return p;
}

double MLT::MISWeight(const std::vector<HitInfo> path, const int pathLength) const {
	//std::cout << "MISWeight" << std::endl;
	int numEyeVertices = path.size();
	const double p_i = pathProbabilityDensity(path, numEyeVertices);
	const double p_all = pathProbabilityDensity(path);

	if(p_i == 0.0f || p_all == 0.0f) {    // Kan man skrive (!p_i || !p_all) bare for at være et jerk?
		return 0.0f;
	} else {
		return std::max(std::min(p_i / p_all, 1.0), 0.0);
	}
}

double MLT::directionToArea(const HitInfo current, const HitInfo next) const {
	//std::cout << "directionToArea" << std::endl;
	const Vector3 dv = next.P - current.P;					// Distance between vertices
	const double d2 = dot(dv, dv);							// Distance squared
	return abs(dot(next.N, dv)) / (d2 * sqrt(d2));			// dot product of next normal and distance divided by d^3
}

// TODO: Bliver ikke brugt
float acceptProb(float x, float y) {
	// T(y > x) / T(x > y)
	return 0;
}
