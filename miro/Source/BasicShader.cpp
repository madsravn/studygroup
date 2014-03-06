#include "BasicShader.h"


BasicShader::BasicShader(Scene& scene, Image* image, Camera* camera)  : scene(scene), img(image), cam(camera) {

}


BasicShader::~BasicShader(void)
{
}

void BasicShader::run() {
	HitInfo hitInfo;
	Vector3 shadeResult = 0;
	Ray ray;

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			shadeResult = Vector3(0.0f);
			for (int AAx = 0; AAx < 2; AAx++){
				for (int AAy = 0; AAy < 2; AAy++) {
					ray = cam->eyeRay(i - 0.25f + (float)AAx/2, j - 0.25f + (float)AAy/2, img->width(), img->height());
					if (scene.trace(hitInfo, ray, 0.0001f)) {
						shadeResult += hitInfo.material->shade(ray, hitInfo, scene, 0, 1) / 4.0;
					}
				}
			}		
			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j / double(img->height())*100.0f);
		fflush(stdout);
	}
}