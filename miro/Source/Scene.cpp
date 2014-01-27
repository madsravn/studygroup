#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "PFMLoader.h"
#include <iostream>
#include <thread>

Scene * g_scene = 0;

const int recDepth = 4;
const int camRays = 1;

Vector3 Scene::getHDRColorFromVector(const Vector3 &direction) const {

	Vector3 ret;
	int pfmWidth = 1500, pfmHeight = 1500;

	float r = (1/PI) * acos(direction.z)/sqrt(pow(direction.x, 2) + pow(direction.y, 2));

    float realx = ((direction.x*r + 1) / 2.0)*pfmWidth;
    float realy = ((direction.y*r + 1) / 2.0)*pfmHeight;
    int estimatedx = floor(realx);
    int estimatedy = floor(realy);
    float area = (estimatedx-realx)*(estimatedy-realy);
    ret += area*pfmImage[estimatedx + estimatedy*pfmWidth];

    estimatedy = ceil(realy);
    area = (estimatedx-realx)*(realy-estimatedy);
    ret += area*pfmImage[estimatedx + estimatedy*pfmWidth];

    estimatedx = ceil(realx);
    area = (realx-estimatedx)*(realy-estimatedy);
    ret += area*pfmImage[estimatedx + estimatedy*pfmWidth];

    estimatedy = floor(realy);
    area = (realx-estimatedx)*(estimatedy-realy);
    ret += area*pfmImage[estimatedx + estimatedy*pfmWidth];

	/*ret = pfmImage[
			(int)(((direction.x * r) + 1) / 2 * pfmWidth) + 
			(int)(((direction.y * r) + 1) / 2 * pfmHeight) * pfmHeight];
            */
	return ret;
}

Vector3 clamp(Vector3 vector, float lowerBound, float upperBound) {
	if (vector.x > upperBound)
		vector.x = upperBound;
	else if (vector.x < lowerBound)
		vector.x = lowerBound;
	if (vector.y > upperBound)
		vector.y = upperBound;
	else if (vector.y < lowerBound)
		vector.y = lowerBound;
	if (vector.z > upperBound)
		vector.z = upperBound;
	else if (vector.z < lowerBound)
		vector.z = lowerBound;
	return vector;
}

Vector3 exp(const Vector3 x) {
	return Vector3(exp(x.x), exp(x.y), exp(x.z));
}

// Can't remember where I found this algorithm, I took it from a project I made two years ago
bool isPointInPolygon(const Vector3 point, const int polygonSides) {
	Vector3* points = new Vector3[polygonSides];
	for (int i = 0; i < polygonSides; i++) {
		points[i] = Vector3(
			cos(2*(PI/polygonSides) * i),	//Polar to cartesian coordinates
			sin(2*(PI/polygonSides) * i),
			0);
	}

	int i, j = polygonSides - 1;

	bool c = false;
	for (i = 0, j = polygonSides-1; i < polygonSides; j = i++) {
		if (((&points[i])->y > point.y  != (&points[j])->y > point.y ) &&
			(point.x < ((&points[j])->x - (&points[i])->y) * (point.y -  (&points[i])->y) / ( (&points[j])->y -  (&points[i])->y) +  (&points[i])->x) )
			c = !c;
	}

	return c;
}

void
	Scene::openGL(Camera *cam)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam->drawGL();

	// draw objects
	for (size_t i = 0; i < m_objects.size(); ++i)
		m_objects[i]->renderGL();

	glutSwapBuffers();
}

void
	Scene::preCalc()
{
	Objects::iterator it;
	for (it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		Object* pObject = *it;
		pObject->preCalc();
	}
	Lights::iterator lit;
	for (lit = m_lights.begin(); lit != m_lights.end(); ++lit)
	{
		PointLight* pLight = *lit;
		pLight->preCalc();
	}

	m_bvh.build(&m_objects);
}

void
	Scene::raytraceImage(Camera *cam, Image *img)
{
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	int pfmWidth = 1500, pfmHeight = 1500;
	pfmImage = readPFMImage("hdr/stpeters_probe.pfm", &pfmWidth, &pfmWidth);

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			ray = cam->eyeRay(i, j, img->width(), img->height());					
			shadeResult = basicShading(ray);

			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}

	printf("Rendering Progress: 100.000%\n");

	debug("done Raytracing!\n");
}

Vector3 Scene::basicShading(const Ray ray) {
	
	HitInfo hitInfo;
	Vector3 shadeResult = 0;
	
	if (trace(hitInfo, ray))
	{
		shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth));
	}
	else {
		//Image based
		shadeResult += getHDRColorFromVector(ray.d);
	}			

	return shadeResult;
}


bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
