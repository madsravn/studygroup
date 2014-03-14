#include "BiPathTracer.h"
#include <algorithm>



BiPathTracer::BiPathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
	for(int i = 0; i < 3*img->height()*img->width(); ++i) {
		picture.push_back(0.0f);
	}

	dummyShader = new Lambert(Vector3(0.0f));
}

BiPathTracer::~BiPathTracer(void)
{
}

void BiPathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	

	double inverseSamples = 1/(double)samples;

	// Paint over the geometry scene
	for(int j = 0; j < img->height(); ++j) {
		for (int i = 0; i < img->width(); i++)
		{
			img->setPixel(i, j, Vector3(0.0f));
		}
		img->drawScanline(j);
		glFinish();
	}

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			double contSum = 0.0;
			ray = cam->eyeRay(i, j, img->width(), img->height());	

			for (int sampleCounter = 0; sampleCounter < samples; sampleCounter++){
				
				std::vector<HitInfo> eyePath = generateEyePath(ray);
				
				for (int light = 0; light < scene.lights()->size(); light++) {					
					Vector3 lightPathResult = Vector3(0.0f);
					
					const PointLight* pLight = scene.lights()->at(light);
					std::vector<HitInfo> lightPath = generateLightPath(pLight->position());
					
					PathContribution pathContribution = calcCombinePaths(eyePath, lightPath);
					/*if (pathContribution.scalarContribution <= 0.0)
						std::cout << "pathContribution <= 0:\t(" << i << ", " << j << ")" << std::endl;*/
					accumulatePathContribution(pathContribution, inverseSamples);			

					contSum += pathContribution.scalarContribution;
				}
			}

			/*if (contSum <= 0.0)
				std::cout << "pathContribution <= 0:\t(" << i << ", " << j << ")" << std::endl;*/
		}

		for (int i = 0; i < img->height(); ++i) {
			img->drawScanline(i);
			glFinish();
		}

		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);
	}

	for (int j = 0; j < img->height(); ++j) {
		img->drawScanline(j);
		glFinish();
	}
}

PathContribution BiPathTracer::calcCombinePaths(const std::vector<HitInfo> eyePath, const std::vector<HitInfo> lightPath) const {
	PathContribution pathContribution;	

	int px, py;

	for (int combinedPathSize = Constants::MinPathLength; combinedPathSize <= std::min(Constants::MaxPathLength, (int)(eyePath.size() + lightPath.size())); combinedPathSize++) {
        // Smallest path is camera to surface (length 2)		
		for (int eyeSubPathSize = 1; eyeSubPathSize <= std::min(combinedPathSize, (int)eyePath.size()); eyeSubPathSize++) {
            
            int lightSubPathSize = combinedPathSize - eyeSubPathSize;			
			if(lightSubPathSize > lightPath.size()) continue;

			std::vector<HitInfo> EyeSubPath = subVector(eyePath, 0, eyeSubPathSize);
			std::vector<HitInfo> reverseLightSubPath = subVector(lightPath, 0, lightSubPathSize);
			std::reverse(reverseLightSubPath.begin(), reverseLightSubPath.end());

			if(!isConnectable(EyeSubPath, reverseLightSubPath)) continue;

			std::vector<HitInfo> combinedPath = concatVectors(EyeSubPath, reverseLightSubPath);

			Vector3 rayToPixelsDir = (combinedPath.at(1).P - combinedPath.at(0).P).normalized();

			HitInfo camHit;
			scene.trace(camHit, Ray(combinedPath.at(1).P, (combinedPath.at(0).P - combinedPath.at(1).P).normalized()), 0.001f);
			if (camHit.t < (combinedPath.at(0).P - combinedPath.at(1).P).length()){ // Visibility test
				//std::cout << "Camera not hit" << std::endl;
				continue;
			}

			cam->rayToPixels(
				Ray(combinedPath.at(0).P, rayToPixelsDir), 
				px, py, img->width(), img->height());

			if (px >= 0 && px <= img->width() && py >= 0 && py <= img->height()) {							
				Vector3 lightPathResult = pathTraceFromPath(combinedPath);
				if (maxVectorValue(lightPathResult) <= 0.0) 
					continue;
				double p = pathProbabilityDensity(combinedPath, eyeSubPathSize, lightSubPathSize);
				if (p <= 0.0) 
					continue;
				double w = MISWeight(combinedPath, combinedPathSize);
				if (w <= 0.0f) 
					continue;

				Contribution contribution(px, py, lightPathResult/* * (w / p)*/);
				pathContribution.colors.push_back(contribution);

				pathContribution.scalarContribution = std::max(pathContribution.scalarContribution, maxVectorValue(contribution.color));
			}			
		}
	}

	return pathContribution;
}

void BiPathTracer::accumulatePathContribution(const PathContribution pathContribution, const double scaling) const {	
	//std::cout << "accumulatePathContribution" << std::endl;
	for (int i = 0; i < pathContribution.colors.size(); i++) {
		Contribution currentColor = pathContribution.colors.at(i);

		const int ix = int(currentColor.x);
		const int iy = int(currentColor.y);

		Vector3 color = currentColor.color * scaling;
		if (ix >= 0 && ix < img->width() && iy >= 0 && iy < img->height()) {	
			int pixelpos = iy*img->width() + ix;

			Vector3 newColor = color + Vector3(picture[3*pixelpos], picture[3*pixelpos+1], picture[3*pixelpos+2]);

			double s = 1.0 / Constants::PathSamples;

			color = newColor;
			picture[3*pixelpos] = color.x;
			picture[3*pixelpos+1] = color.y;
			picture[3*pixelpos+2] = color.z;
			img->setPixel(ix, iy, color * s);

			//std::cout << ix << ", " << iy << " = " << color << std::endl;

			/*img->drawPixel(ix, iy);
			glFinish();*/
		}
	}
}

bool BiPathTracer::isConnectable(const std::vector<HitInfo> eyePath, const std::vector<HitInfo> lightPath) const {
	//std::cout << "isConnectable(" << eyePath.size() << ", " << lightPath.size() << ")" << std::endl;

	if(lightPath.size() == 0) {
        //std::cout << "Returning true due to lightPath.size() == 0" << std::endl;
		return true;
    }
	HitInfo hitInfo;
	HitInfo lastEyePoint = eyePath.at(eyePath.size() - 1);
	HitInfo lastLightPoint = lightPath.at(lightPath.size() - 1);
	Ray connectorRay = Ray(eyePath.at(eyePath.size() - 1).P, (lastLightPoint.P - lastEyePoint.P).normalized());
	
	scene.trace(hitInfo, connectorRay, 0.001f);
	if(hitInfo.t >= (lastEyePoint.P - lastLightPoint.P).length()) {		
		return true;
	} else {
		return false;
	}
}

Vector3 BiPathTracer::pathTraceFromPath(std::vector<HitInfo> path) const {
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);

	if (path.size() >= 2 && path.at(1).material != nullptr) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}

	return shadeResult;
}

std::vector<HitInfo> BiPathTracer::generateEyePath(const Ray& eyeRay) const {
	std::vector<HitInfo> path = std::vector<HitInfo>();
	path.push_back(HitInfo(0.0f, eyeRay.o, eyeRay.d));	// Eye position
	return path;			// Test uden eye path

	Ray ray = eyeRay;
	HitInfo hitInfo;
	for (int i = 0; i < Constants::MaxPathLength; i++)
	{
		if(scene.trace(hitInfo, ray, 0.001f)) {
			path.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo);
		} else {
			break;
		}
	}
	return path;
}

std::vector<HitInfo> BiPathTracer::generateEyePath(const Ray& eyeRay, const MarkovChain& MC) const {
	std::vector<HitInfo> path = std::vector<HitInfo>();
	path.push_back(HitInfo(0.0f, eyeRay.o, eyeRay.d));	// Eye position

	Ray ray = eyeRay;
	HitInfo hitInfo;
	for (int i = 0; i < Constants::MaxPathLength; i++)
	{
		if(scene.trace(hitInfo, ray, 0.001f)) {
			path.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo, MC);
		} else {
			break;
		}
	}
	return path;
}

std::vector<HitInfo> BiPathTracer::generateLightPath(const Vector3 lightPos) const {
	std::vector<HitInfo> lightPath = std::vector<HitInfo>();
	//return lightPath;		// Test uden light path

	Vector3 lightDir = generateRandomRayDirection();
	/*HitInfo lightHit = HitInfo(0.0f, lightPos, lightDir);
	lightHit.material = dummyShader;
	lightPath.push_back(lightHit);*/

	Ray ray = Ray(lightPos, lightDir);
	HitInfo hitInfo;

	for (int i = 0; i < Constants::MaxPathLength; i++) {
		if(scene.trace(hitInfo, ray, 0.001f)) {
			lightPath.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo);
		} else {
			break;
		}
	}	

	return lightPath;
}

std::vector<HitInfo> BiPathTracer::generateLightPath(const Vector3 lightPos, const MarkovChain& MC) const {
	std::vector<HitInfo> lightPath = std::vector<HitInfo>();
	Vector3 lightDir = generateRandomRayDirection(MC.getNext(), MC.getNext());
	/*HitInfo lightHit = HitInfo(0.0f, lightPos, lightDir);
	lightHit.material = dummyShader;
	lightPath.push_back(lightHit);*/

	Ray ray = Ray(lightPos, lightDir);
	HitInfo hitInfo;

	for (int i = 0; i < Constants::MaxPathLength; i++) {
		if(scene.trace(hitInfo, ray, 0.001f)) {
			lightPath.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo, MC);
		} else {
			break;
		}
	}
	return lightPath;
}

PathContribution BiPathTracer::calcPathContribution(const MarkovChain& MC) const {
	MarkovChain normChain(img->width(), img->height());

	std::vector<HitInfo> eyePath = generateEyePath(cam->randomRay(img->width(), img->height(), normChain), MC);

	const PointLight* pLight = scene.lights()->at((int)MC.getNext() % scene.lights()->size()); // Choose random light
	std::vector<HitInfo> lightPath = generateLightPath(pLight->position(), MC);

	return calcCombinePaths(eyePath, lightPath);
}

// Probability density for path with all numbers of vertices
double BiPathTracer::pathProbabilityDensity(const std::vector<HitInfo> path) const {
	//std::cout << "pathProbabilityDensity" << std::endl;
	double p = 0.0f;	
	for (int numEyeVertices = 0; numEyeVertices <= path.size(); numEyeVertices++) {
		int numLightVertices = path.size() - numEyeVertices;
		p += pathProbabilityDensity(path, numEyeVertices, numLightVertices);										//Hvis vi skal bruge TKhanAdder ligesom Toshiya skal den tilf�jes her
	}
	return p;
}

double BiPathTracer::pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices, int numLightVertices) const {
	double p = 1.0;

	// sampling from the eye
	for (int i = 1; i < numEyeVertices - 1; i++) {
		if (i == 0) {

		} else if (i == 1) {  // First hit
			p *= 1.0 / double(img->width() * img->height());						// divided by image size
			Vector3 direction = (path.at(i).P - path.at(i - 1).P).normalized();		// Direction from first to second hit
			double cosTheta = dot(direction, cam->viewDir());						// Cosine of angle from camera
			double distanceToScreen = cam->getDistance() / cosTheta;				// Distance to screen
			distanceToScreen = distanceToScreen * distanceToScreen;					// Distance to screen squared
			p /= (cosTheta / distanceToScreen);										// Divided by cosine of angle divided by distance to screen squared	
		}
		else {																	// Other hits
			// PDF of sampling ith vertex
			Vector3 directionIn = (path.at(i - 1).P - path.at(i).P).normalized();
			Vector3 directionOut = (path.at(i + 1).P - path.at(i).P).normalized();
			p *= path.at(i).material->getPDF(directionIn, directionOut, path.at(i).N);
		}		
		p *= directionToArea(path.at(i), path.at(i + 1));
	}

	// sampling from light source
	if (p != 0.0) {		

		for (int i = 2; i < numLightVertices - 1; i++) {
			if (i == 2) {
				HitInfo firstPoint = path.at(path.size() - 1);
				HitInfo secondPoint = path.at(path.size() - 2);
				Vector3 firstLightDir = (secondPoint.P - firstPoint.P).normalized(); // direction from light to first light hit				
				p *= abs(dot(firstLightDir, firstPoint.N)) / PI;				
			} else {
				HitInfo iPoint = path.at(path.size() - i);
				Vector3 directionIn  = (path.at(path.size() - (i - 1)).P - iPoint.P).normalized();
				Vector3 directionOut = (path.at(path.size() - (i + 1)).P - iPoint.P).normalized();
				
				p *= iPoint.material->getPDF(directionIn, directionOut, iPoint.N);				
			}
			p *= directionToArea(path.at(i), path.at(i + 1));
		}
	}
	return p;
}

double BiPathTracer::MISWeight(const std::vector<HitInfo> path, const int pathLength) const {
	const double p_i = pathProbabilityDensity(path, path.size(), path.size());
	const double p_all = pathProbabilityDensity(path);

	if (!p_i || !p_all) {    // Kan man skrive (!p_i || !p_all) bare for at v�re et jerk?
		return 0.0f;
	}
	else {
		return std::max(std::min(p_i / p_all, 1.0), 0.0);
	}
}