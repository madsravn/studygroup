#include "BiPathTracer.h"



BiPathTracer::BiPathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples) : scene(scene), img(image), cam(camera), samples(pathSamples) {
}

BiPathTracer::~BiPathTracer(void)
{
}

void BiPathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	double inverseSamples = 1/(double)samples;

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			ray = cam->eyeRay(i, j, img->width(), img->height());			
			for (int sampleCounter = 0; sampleCounter < samples; sampleCounter++){

				std::vector<HitInfo> eyePath = generateEyePath(ray);
				
				for (int light = 0; light < scene.lights()->size(); light++) {
					Vector3 lightPathResult = Vector3(0.0f);
					
					const PointLight* pLight = scene.lights()->at(light);
					std::vector<HitInfo> lightPath = generateLightPath(pLight->position());

					PathContribution pathContribution = calcCombinePaths(eyePath, lightPath);
					
					for (int c = 0; c < pathContribution.colors.size(); c++) {

						Contribution contribution = pathContribution.colors.at(c);
												
						// TODO: Update pixels
						// pixelColor += contribution.color * pathContribution.scalarContribution * inverseSamples;
						// assert pixel is in image
						// updatePixel(contribution.x, contribution.y, contribution.color);
					}
				}
			}
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);
	}
}

PathContribution BiPathTracer::calcCombinePaths(std::vector<HitInfo> eyePath, std::vector<HitInfo> lightPath) const {
	PathContribution pathContribution = PathContribution();		

	for (int combinedPathSize = 2; combinedPathSize < 12; combinedPathSize++) {		
		for(int eyePathSize = 2; eyePathSize < eyePath.size(); eyePathSize++) {
			int lightSubPathSize = combinedPathSize - eyePathSize;
			std::vector<HitInfo> EyeSubPath = subVector(eyePath, 0, eyePathSize);
			std::vector<HitInfo> reverseLightSubPath = subVector(lightPath, 0, lightSubPathSize);
			std::reverse(reverseLightSubPath.begin(), reverseLightSubPath.end());

			if(isConnectable(EyeSubPath, reverseLightSubPath));

			std::vector<HitInfo> combinedPath = concatVectors(EyeSubPath, reverseLightSubPath);

			int px, py;
			cam->rayToPixels(
				Ray(combinedPath.at(0).P, (combinedPath.at(0).P - combinedPath.at(1).P).normalized()), 
				px, py, img->width(), img->height());

			Vector3 lightPathResult = pathTraceFromPath(combinedPath);
			Contribution contribution = Contribution(px, py, lightPathResult);
			pathContribution.colors.push_back(contribution);

			pathContribution.scalarContribution = std::max(pathContribution.scalarContribution, max(contribution.color));
		}
	}
	
	Vector3 lightPathResult = Vector3(0.0f);

	if(isConnectable(eyePath, lightPath)) {
		std::vector<HitInfo> combinedPath = concatVectors(eyePath, lightPath);

		lightPathResult += pathTraceFromPath(combinedPath);
	}

	return pathContribution;
}

bool BiPathTracer::isConnectable(std::vector<HitInfo> eyePath, std::vector<HitInfo> lightPath) const {
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