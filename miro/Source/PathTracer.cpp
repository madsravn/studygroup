
#include "PathTracer.h"


PathTracer::PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples)  : scene(scene), img(image), cam(camera), samples(pathSamples) {

}

void PathTracer::run() {
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	double inverseSamples = 1/(double)samples;

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			shadeResult = Vector3(0.0f);
			for (int sampleCounter = 0; sampleCounter < samples; sampleCounter++){
				ray = cam->eyeRay(i, j, img->width(), img->height());																

				if (scene.trace(hitInfo, ray, 0.0001f)) {
					shadeResult += hitInfo.material->shade(ray, hitInfo, scene, 0) * inverseSamples;		
				}
				img->setPixel(i, j, shadeResult) ;
			}
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);
	}
}

PathTracer::~PathTracer(void)
{
}
