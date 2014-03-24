#include "PathTracer.h"
#include "Timer.h"

bool buildPath = true;	// Build path before shading
bool AA = false;

PathTracer::PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples)  : scene(scene), img(image), cam(camera), samples(pathSamples) {
}

PathTracer::~PathTracer(void)
{
}

void PathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	double inverseSamples = 1/(double)samples;

	std::cout << "inverseSamples = " << inverseSamples << std::endl;

	Timer t;
	t.start();
	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			shadeResult = Vector3(0.0f);						

			if (!AA) {
				ray = cam->eyeRay(i, j, img->width(), img->height());
				for (int sampleCounter = 0; sampleCounter < samples; sampleCounter++){

					Vector3 traceResult(0.0f);
					Vector3 pathResult(0.0f);

					std::vector<HitInfo> path = generatePath(ray);
					if (path.size() < 2) break;

					PathContribution contribution = calcPathContribution(path);
					for (int c = 0; c < contribution.colors.size(); c++)
					{
						Contribution cont = contribution.colors.at(c);
						pathResult += cont.color;
					}
					shadeResult += pathResult;
				}
			}
			else {
				for (int AAx = 0; AAx < 2; AAx++) {
					for (int AAy = 0; AAy < 2; AAy++) {
						ray = cam->eyeRay(i - 0.25f + (float)AAx/2, j - 0.25f + (float)AAy/2, img->width(), img->height());
						for (int sampleCounter = 0; sampleCounter < samples / 4; sampleCounter++){

							Vector3 traceResult(0.0f);
							Vector3 pathResult(0.0f);

							std::vector<HitInfo> path = generatePath(ray);
							if (path.size() < 2) break;

							PathContribution contribution = calcPathContribution(path);
							for (int c = 0; c < contribution.colors.size(); c++)
							{
								Contribution cont = contribution.colors.at(c);
								pathResult += cont.color;
							}
							shadeResult += pathResult;
						}
					}
				}
			}

			shadeResult *= inverseSamples;
			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);		
	}	
	renderSeconds = (t.duration().count() / 1000);
	char str[1024];
	sprintf(str, "PT_%d_%ds", Constants::PathSamples, (t.duration().count() / 1000));
	scene.writeImg(str);
}

Vector3 PathTracer::pathTraceFromPath(std::vector<HitInfo> path) const{	
	// Recursive shading
	Vector3 shadeResult(0.0f);
	if (path.size() >= 2) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}

	return shadeResult;
}

// Used by MLT
PathContribution PathTracer::calcPathContribution(const std::vector<HitInfo> path) const {
	PathContribution result = PathContribution();

	if (path.size() < 2) return result;

	// Pixel Position
	int px = -1, py = -1;
	Vector3 direction = (path.at(1).P - path.at(0).P).normalized();
	cam->rayToPixels(Ray(cam->eye(), direction), px, py, img->width(), img->height());
	
	// Path Probability Density
	/*double probabilityDensity = pathProbabilityDensity(path, path.size());
	if (probabilityDensity <= 0.0f) return result;*/
	
	// Color
	Vector3 throughput = pathTraceFromPath(path);
	Vector3 color = throughput;
	//color = Vector3(.5f);
	// Assert color is positive
	if (maxVectorValue(color) <= 0.0f) return result;
	result.colors.push_back(Contribution(px, py, color));	
	result.scalarContribution = std::max(maxVectorValue(color), result.scalarContribution);	

	return result;
}

// Used by MLT
PathContribution PathTracer::calcPathContribution(const MarkovChain& MC) const {
	MarkovChain normChain(img->width(), img->height());
	std::vector<HitInfo> path = generatePath(cam->randomRay(img->width(), img->height(), normChain), MC);
	return calcPathContribution(path);
}

// Probability density for path with all numbers of vertices
double PathTracer::pathProbabilityDensity(const std::vector<HitInfo> path) const {
	double p = 0.0f;
	for (int numEyeVertices = 0; numEyeVertices <= path.size(); numEyeVertices++) {
		p += pathProbabilityDensity(path, numEyeVertices);										//Hvis vi skal bruge TKhanAdder ligesom Toshiya skal den tilføjes her
	}
	return p;
}

double PathTracer::pathProbabilityDensity(const std::vector<HitInfo> path, int numEyeVertices) const {
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
		}
		else {																		// Other hits
			// PDF of sampling ith vertex
			Vector3 directionIn = (path.at(i - 1).P - path.at(i).P).normalized();
			Vector3 directionOut = (path.at(i + 1).P - path.at(i).P).normalized();
			p *= path.at(i).material->getPDF(directionIn, directionOut, path.at(i).N);
		}
		p *= directionToArea(path.at(i), path.at(i + 1));
	}
	return p;
}

std::vector<HitInfo> PathTracer::generatePath(const Ray& eyeRay) const {
	std::vector<HitInfo> path = std::vector<HitInfo>();
	path.push_back(HitInfo(0.0f, eyeRay.o, eyeRay.d));	// Eye position

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

// Used by MLT
std::vector<HitInfo> PathTracer::generatePath(const Ray& eyeRay, const MarkovChain& MC) const {
	std::vector<HitInfo> path = std::vector<HitInfo>();
	path.push_back(HitInfo(0.0f, eyeRay.o, eyeRay.d));	// Eye position

	Ray ray = eyeRay;
	HitInfo hitInfo;
	for (int i = 0; i < Constants::MaxPathLength; i++)
	{
		if (scene.trace(hitInfo, ray, 0.001f)) {
			path.push_back(hitInfo);
			ray = hitInfo.material->bounceRay(ray, hitInfo, MC);
		}
		else {
			break;
		}
	}
	return path;
}