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
	int x, y;

	cam->rayToPixels(eyeRay, x, y, img->width(), img->height());

	//std::cout << "(" << x << ", " << y << ")" << std::endl;

	img->setPixel(x,y,Vector3(0.0f));
	img->drawPixel(x, y);
	glFinish();

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
	for (int i = 0; i < 10000; i++) {
		fprintf(stdout, "\rPSMLT Initializing: %5.2f", 100.0 * i / (10000));		
        fflush(stdout);
		MarkovChain normChain(img->width(), img->height());
		b += calcPathContribution(generateEyePath(cam->randomRay(img->width(), img->height(), normChain), MC)).scalarContribution;
	}
    printf("\n");
	b /= double(10000);	// average
	
    bool running = true;
    MarkovChain current(img->width(), img->height());
    MarkovChain proposal(img->width(), img->height());

    // Initialize current
    Ray tRay = cam->randomRay(img->width(), img->height(), current);
    std::vector<HitInfo> tPath = generateEyePath(tRay, current);
    current.contribution = calcPathContribution(tPath);
    const int count = 512*512;
    int i = 0;

    // Paint over the geometry scene
    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
        glFinish();
    }
    while( true ) {
		//std::cout << i << std::endl;
		
        double isLargeStepDone;
        if(rnd() <= LargeStepProb) {
            isLargeStepDone = 1.0;
            proposal = current.large_step(img->width(), img->height());
        } else {
            isLargeStepDone = 0.0;
            proposal = current.mutate(img->width(), img->height());
        }

        Ray ray = cam->randomRay(img->width(), img->height(), current);	// TODO: Efter noget tid laver denne linje den samme ray altid.
        cam->rayToPixels(ray, ai, bi, img->width(), img->height());

        std::vector<HitInfo> path = generateEyePath(ray, current); // TODO: Skal den her evt. være et eller andet

		proposal.contribution = calcPathContribution(path);

		//std::cout << proposal.contribution.scalarContribution << std::endl;
        
		double a = acceptProb(current, proposal);

		//std::cout << a << std::endl;

		// TODO: accumulate samples
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

        if(rnd() <= a) {
            current = proposal;
        }
        
		i++;
        
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
	for (int i = 1; i < pathContribution.colors.size(); i++) {    // Start at first hit, [0] is camera
		Contribution currentColor = pathContribution.colors.at(i);

		const int ix = int(currentColor.x);
		const int iy = int(currentColor.y);
		
		Vector3 color = currentColor.color * scaling;		// TODO: Skal være fladens farve * scaling
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {					        
            color = color + picture[iy*img->width() + ix];
            (picture.at(iy*img->width() + ix)).set(color);
			img->setPixel(ix, iy, color);

			//std::cout << "(" << ix << ", " << iy << ") = " << color << std::endl;

			////////
			img->drawPixel(ix, iy);
			glFinish();
		}
	}
}

PathContribution MLT::calcPathContribution(const std::vector<HitInfo> path) const {

	//std::cout << "calcPathContribution" << std::endl;
	PathContribution result = PathContribution();

	if (path.size() < 2)
		return result;

	int px = -1, py = -1;
	Vector3 direction = (path.at(1).P - path.at(0).P).normalized();

	cam->rayToPixels(Ray(cam->eye(), direction), px, py, img->width(), img->height());

	//for (int pathLength = 1; pathLength < std::min(13, (int)path.size()); pathLength++) {       // TODO: Vi skal lige være sikre på de to intervaller her		

		std::vector<HitInfo> subPath = subVector(path, 0, path.size());	
		
		// TODO: Set px and py based on the direction
		//calcCoordinates(subPath, px, py);

		Vector3 throughput = pathTraceFromPath(path); //pathTroughput(subPath);

		double probabilityDensity = pathProbabilityDensity(subPath, path.size());	// Denne bliver også kørt inde i MISWeight, overflødigt? Nææh
		if (probabilityDensity <= 0.0f) return result;

		double weight = MISWeight(subPath, path.size());
		if (weight <= 0.0f) return result;

		Vector3 color = throughput * (weight / probabilityDensity);

		// Assert color is positive
		if (maxVectorValue(color) <= 0.0f) return result;

		result.colors.push_back(Contribution(px, py, color));
		result.scalarContribution = std::max(maxVectorValue(color), result.scalarContribution);
	//}
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
		double cont_proposal = proposal.contribution.scalarContribution;	// TODO: Kan blive 0. Det skal den nok ikke være
		double cont_current = current.contribution.scalarContribution;
		a = cont_proposal / cont_current;
		//std::cout << a << std::endl;
        //std::cout << "a = " << a << std::endl;
		a = std::min(1.0, a); // Clamp value
		a = std::max(a, 0.0);
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
