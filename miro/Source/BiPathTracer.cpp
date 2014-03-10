#include "BiPathTracer.h"
#include <algorithm>



BiPathTracer::BiPathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
	for(int i = 0; i < 3*img->height()*img->width(); ++i) {
		picture.push_back(0.0f);
	}
}

BiPathTracer::~BiPathTracer(void)
{
}

void BiPathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	double inverseSamples = 1/(double)samples;
	// Paint over the geometry scene
	for(int j = 0; j < img->height(); ++j) {
		img->drawScanline(j);
		glFinish();
	}
	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			
			ray = cam->eyeRay(i, j, img->width(), img->height());	

			int px, py;

			cam->rayToPixels(ray, px, py, img->width(), img->height());
			for (int sampleCounter = 0; sampleCounter < samples; sampleCounter++){
				
				std::vector<HitInfo> eyePath = generateEyePath(ray);
				
				for (int light = 0; light < scene.lights()->size(); light++) {					
					Vector3 lightPathResult = Vector3(0.0f);
					
					const PointLight* pLight = scene.lights()->at(light);
					std::vector<HitInfo> lightPath = generateLightPath(pLight->position());
					
					PathContribution pathContribution = calcCombinePaths(eyePath, lightPath);
					accumulatePathContribution(pathContribution, inverseSamples);
				
				}
			}
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
	PathContribution pathContribution = PathContribution();		

	//std::cout << "calcCombinePaths(" << eyePath.size() << ", " << lightPath.size() << ")" << std::endl;

	int px, py;

	for (int combinedPathSize = 2; combinedPathSize <= std::min(Constants::maxRecDepth, (int)(eyePath.size() + lightPath.size())); combinedPathSize++) {		
		for(int eyeSubPathSize = 1; eyeSubPathSize <= std::min(combinedPathSize, (int)eyePath.size()); eyeSubPathSize++) {    // Smallest path is camera to surface (length 2)		
			int lightSubPathSize = combinedPathSize - eyeSubPathSize;
			
			if(lightSubPathSize > lightPath.size()) continue;		

			std::vector<HitInfo> EyeSubPath = subVector(eyePath, 0, eyeSubPathSize);
			std::vector<HitInfo> reverseLightSubPath = subVector(lightPath, 0, lightSubPathSize);
			std::reverse(reverseLightSubPath.begin(), reverseLightSubPath.end());

			if(!isConnectable(EyeSubPath, reverseLightSubPath)) continue;

			std::vector<HitInfo> combinedPath = concatVectors(EyeSubPath, reverseLightSubPath);

			Vector3 rayToPixelsDir = (combinedPath.at(1).P - combinedPath.at(0).P).normalized();
			cam->rayToPixels(
				Ray(combinedPath.at(0).P, rayToPixelsDir), 
				px, py, img->width(), img->height());

			if (px >= 0 && px <= img->width() && py >= 0 && py <= img->height()) {							
				Vector3 lightPathResult = pathTraceFromPath(combinedPath);
				Contribution contribution = Contribution(px, py, lightPathResult);
				pathContribution.colors.push_back(contribution);

				pathContribution.scalarContribution = std::max(pathContribution.scalarContribution, max(contribution.color));
			}			
		}
	}

	return pathContribution;
}

void BiPathTracer::accumulatePathContribution(const PathContribution pathContribution, const double scaling) const {	
	//std::cout << "accumulatePathContribution" << std::endl;
	for (int i = 0; i < pathContribution.colors.size(); i++) {    // Start at first hit, [0] is camera
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

	if(lightPath.size() == 0)
		return true;
	HitInfo hitInfo;
	HitInfo lastEyePoint = eyePath.at(eyePath.size() - 1);
	HitInfo lastLightPoint = lightPath.at(lightPath.size() - 1);
	Ray connectorRay = Ray(eyePath.at(eyePath.size() - 1).P, (lastLightPoint.P - lastEyePoint.P).normalized());
	
	scene.trace(hitInfo, connectorRay);
	if(hitInfo.t >= (lastEyePoint.P - lastLightPoint.P).length()) {		
		return true;
	} else {
		return false;
	}
}

Vector3 BiPathTracer::pathTraceFromPath(std::vector<HitInfo> path) const {
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);

	if (path.size() >= 2) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}

	return shadeResult;
}

std::vector<HitInfo> BiPathTracer::generateEyePath(const Ray& eyeRay) const {
	std::vector<HitInfo> path = std::vector<HitInfo>();
	path.push_back(HitInfo(0.0f, eyeRay.o, eyeRay.d));	// Eye position

	Ray ray = eyeRay;
	HitInfo hitInfo;
	for (int i = 0; i < Constants::maxRecDepth; i++)
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

std::vector<HitInfo> BiPathTracer::generateLightPath(const Vector3 lightPos) const {
	std::vector<HitInfo> lightPath = std::vector<HitInfo>();
	Vector3 lightDir = generateRandomRayDirection();	
	//lightPath.push_back(HitInfo(0.0f, lightPos, lightDir));

	Ray ray = Ray(lightPos, lightDir);
	HitInfo hitInfo;

	for (int i = 0; i < Constants::maxRecDepth; i++) {
		if(scene.trace(hitInfo, ray, 0.001f)) {
			lightPath.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo);
		} else {
			break;
		}
	}	

	return lightPath;
}
