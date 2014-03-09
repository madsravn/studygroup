#include "MLT.h"
#include "Constants.h"
#include <limits>

const int maxRecDepth  = Constants::MaxPathLength;
int samps = 0;
const int biasSamples = 1000000;


MLT::MLT(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
    MC.imageWidth = image->width();
    MC.imageHeight = image->height();
    for(int i = 0; i < 3*img->height()*img->width(); ++i) {
        picture.push_back(0.0f);
    }

    for(int i = 0; i < img->height(); ++i) {
        for(int j = 0; j < img->width(); ++j) {
            img->setPixel(j,i, Vector3(0.0f));
        }
    }

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
   
    const int INTMAXHALF = std::numeric_limits<int>::max() / 2;

    double LargeStepProb = 0.3;

	double b = 1.0f;
	// Estimate normalization constant
	for (int i = 0; i < biasSamples; i++) {
		fprintf(stdout, "\rPSMLT Initializing: %5.2f", 100.0 * i / (biasSamples));
        fflush(stdout);
		MarkovChain normChain(img->width(), img->height());        
		b += calcPathContribution(generateEyePath(cam->randomRay(img->width(), img->height(), normChain), MC)).scalarContribution;
	}
    printf("\n");
	b /= double(biasSamples);	// average
	
    bool running = true;
    MarkovChain current(img->width(), img->height());
    MarkovChain proposal(img->width(), img->height());

    // Initialize current
    Ray tRay = cam->randomRay(img->width(), img->height(), current);
    std::vector<HitInfo> tPath = generateEyePath(tRay, current);
    current.contribution = calcPathContribution(tPath);
    int count = 0;
    int i = 0;

    // Paint over the geometry scene
    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }
    while( count < 500 ) {
		samps++;
				
        double isLargeStepDone;
        if(rnd() <= LargeStepProb) {
            isLargeStepDone = 1.0;
            proposal = current.large_step(img->width(), img->height());
        } else {
            isLargeStepDone = 0.0;
            proposal = current.mutate(img->width(), img->height());
        }

		std::vector<HitInfo> path = generateEyePathFromChain(proposal);		
		
		proposal.contribution = calcPathContribution(path);

		double a = acceptProb(current, proposal);

		// accumulate samples
		if (proposal.contribution.scalarContribution > 0.0f)
			accumulatePathContribution(proposal.contribution, 
				(a + isLargeStepDone)/
				(proposal.contribution.scalarContribution/b + isLargeStepDone)
			);
		if (current.contribution.scalarContribution > 0.0f)
			accumulatePathContribution(current.contribution, 
				(1.0 - a)/
				(current.contribution.scalarContribution/b + isLargeStepDone)
			);

		/*std::cout << "a = " << a << "\tisLargeStepDone = " << isLargeStepDone << std::endl;
		std::cout << "current scalar = " << current.contribution.scalarContribution << "proposal scalar = " << proposal.contribution.scalarContribution << std::endl;*/

        if(rnd() <= a) {
            current = proposal;
        }
        
		i++;
		if(i % 100000 == 0) {

			std::cout << "samps = " << samps << std::endl;

			i = 0;
            count++;
		    for(int j = 0; j < img->height(); ++j) {
		        img->drawScanline(j);
		        glFinish();
		    }
            //std::cout << "PIXEL (262,78) = " << img->getPixel(262,78) << std::endl;
            //std::cout << "picture[] = " << picture[3*(78*img->width() + 262)] << ", "  << picture[3*(78*img->width() + 262) + 1] << ", "  << picture[3*(78*img->width() + 262) + 2] << std::endl;

		}
        
        //printf("Rendering Progress: %.3f%%\r", i/float(count)*100.0f);
        //fflush(stdout);
    }


	double s = double(img->width() * img->height()) / double(samps);
	
    for(int j = 0; j < img->height(); ++j) {
		for (int i = 0; i < img->width(); ++i) {

			int pixelpos = j*img->width() + i;

			Vector3 color = Vector3(picture[3 * pixelpos], picture[3 * pixelpos + 1], picture[3 * pixelpos + 2]);
			img->setPixel(i, j, color * s);
		}
		img->drawScanline(j);
		glFinish();
    }
}

 //Recursive path tracing
void MLT::tracePath(std::vector<HitInfo>& path, const Ray &ray, int recDepth, const MarkovChain& MC, bool log) const {
	//std::cout << "tracePath" << std::endl;
	
	if(recDepth >= maxRecDepth)	return;
	
	HitInfo hit;

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
	//std::cout << "initialPath" << std::endl;
	return std::vector<HitInfo>();
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

void MLT::accumulatePathContribution(const PathContribution pathContribution, const double scaling) {
	//std::cout << "accumulatePathContribution" << std::endl;
	for (int i = 0; i < pathContribution.colors.size(); i++) {    // Start at first hit, [0] is camera
		Contribution currentColor = pathContribution.colors.at(i);

		const int ix = int(currentColor.x);
		const int iy = int(currentColor.y);
		
		Vector3 color = currentColor.color * scaling;
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {	
            int pixelpos = iy*img->width() + ix;

			Vector3 newColor = color + Vector3(picture[3*pixelpos], picture[3*pixelpos+1], picture[3*pixelpos+2]);

			if(newColor.x < color.x || newColor.y < color.y || newColor.z < color.z)
				std::cout << "new color is darker. Old color: " << color << "\tNew color: " << newColor <<std::endl;

			

			color = newColor;
            picture[3*pixelpos] = color.x;
            picture[3*pixelpos+1] = color.y;
            picture[3*pixelpos+2] = color.z;
			//img->setPixel(ix, iy, color * s);
		}
	}
}

PathContribution MLT::calcPathContribution(const std::vector<HitInfo> path) const {

	//std::cout << "calcPathContribution" << std::endl;
	PathContribution result = PathContribution();

	if (path.size() < 2) return result;

	int px = -1, py = -1;
	Vector3 direction = (path.at(1).P - path.at(0).P).normalized();

	cam->rayToPixels(Ray(cam->eye(), direction), px, py, img->width(), img->height());	

	Vector3 throughput = pathTraceFromPath(path);

	double probabilityDensity = pathProbabilityDensity(path, path.size());	// Denne bliver også kørt inde i MISWeight, overflødigt? Nææh
	if (probabilityDensity <= 0.0f) return result;

	double weight = MISWeight(path, path.size());
	if (weight <= 0.0f) return result;

	Vector3 color = throughput * (weight / probabilityDensity);

	// Assert color is positive
	if (maxVectorValue(color) <= 0.0f) return result;

	result.colors.push_back(Contribution(px, py, color));
	result.scalarContribution = std::max(maxVectorValue(color), result.scalarContribution);
	
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
			Vector3 direction = (path.at(i).P - path.at(i - 1).P).normalized();		// Direction from first to second hit
			double cosTheta = dot(direction, cam->viewDir());						// Cosine of angle from camera
			double distanceToScreen = cam->getDistance() / cosTheta;				// Distance to screen
			distanceToScreen = distanceToScreen * distanceToScreen;					// Distance to screen squared
			p /= (cosTheta / distanceToScreen);										// Divided by cosine of angle divided by distance to screen squared	

			if ( p < 0.0f)
				std::cout << "P is negative, I think this is bad: " <<p << std::endl;

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

double MLT::acceptProb(MarkovChain& current, MarkovChain& proposal) const {
	// T(y > x) / T(x > y)	
	double a = 1.0;
	if (current.contribution.scalarContribution > 0.0){
		double cont_proposal = proposal.contribution.scalarContribution;
		double cont_current = current.contribution.scalarContribution;
		a = clamp(cont_proposal / cont_current, 0.0, 1.0);
	}
	return a;
}

void MLT::calcCoordinates(std::vector<HitInfo> path, int &px, int &py) const {
	Vector3 direction;	

	if (path.size() >= 2) {				
		direction = (path.at(1).P - path.at(0).P).normalized();
		cam->rayToPixels(Ray(cam->eye(), direction), px, py, img->width(), img->height());
	}	
}

// Trace path from eye
std::vector<HitInfo> MLT::generateEyePathFromChain(MarkovChain chain) const {	
	Ray ray = cam->randomRay(img->width(), img->height(), chain);

	return generateEyePath(ray, chain);
}

/*
tentativeTransitionFunction(x -> y){
	make tentative sample X'_i

	if (acceptProb(X_i-1, X'_i)) {
		return X'_i;
	} else {
		return X_i-i;
	}
}
*/
