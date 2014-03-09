#include "PathTracer.h"

bool buildPath = true;	// Build path before shading

PathTracer::PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples)  : scene(scene), img(image), cam(camera), samples(pathSamples) {

}

void PathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	double inverseSamples = 1/(double)samples;

	std::cout << "inverseSamples = " << inverseSamples << std::endl;

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			shadeResult = Vector3(0.0f);			

			for (int AAx = 0; AAx < 2; AAx++) {
				for (int AAy = 0; AAy < 2; AAy++) {
					ray = cam->eyeRay(i - 0.25f + (float)AAx/2, j - 0.25f + (float)AAy/2, img->width(), img->height());
					for (int sampleCounter = 0; sampleCounter < samples/4; sampleCounter++){
						
						Vector3 traceResult = Vector3(0.0f);
						Vector3 pathResult = Vector3(0.0f);
						if (!buildPath) {
							if (scene.trace(hitInfo, ray, 0.0001f)) {
								traceResult = hitInfo.material->shade(ray, hitInfo, scene, 0, Constants::maxRecDepth, 0);		
								shadeResult += traceResult;
							}
						} else {
							std::vector<HitInfo> path = generatePath(ray);
							pathResult = pathTraceFromPath(path);
							shadeResult += pathResult;					
						}
					}
				}
			}

			shadeResult *= inverseSamples / 4;

			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);
	}
}


std::vector<HitInfo> PathTracer::generatePath(const Ray& eyeRay) const {
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


Vector3 PathTracer::pathTraceFromPath(std::vector<HitInfo> path) const{	
	// Recursive shading
	Vector3 shadeResult = Vector3(0.0f);

	if (path.size() >= 2) {
		shadeResult += path.at(1).material->shade(path, 1, scene);			
	}

	return shadeResult;
}

PathTracer::~PathTracer(void)
{
}
