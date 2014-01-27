#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "PFMLoader.h"
#include <iostream>

Scene * g_scene = 0;

const int recDepth = 4;
const int camRays = 1;

Vector3 Scene::getHDRColorFromVector(const Vector3 direction) const {

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

Vector3 exp(Vector3 x) {
	return Vector3(exp(x.x), exp(x.y), exp(x.z));
}

// Can't remember where I found this algorithm, I took it from a project I made two years ago
bool isPointInPolygon(Vector3 point, const int polygonSides) {
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
	for (it = m_objects.begin(); it != m_objects.end(); it++)
	{
		Object* pObject = *it;
		pObject->preCalc();
	}
	Lights::iterator lit;
	for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
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

			Ray referenceRay = cam->eyeRay(i, j, img->width(), img->height());	// TODO: make a plane instead of just distance			

			// camRays control depth of field
			for (int camRayCounter = 0; camRayCounter < camRays; camRayCounter++) {

				Vector3 point = Vector3(1.f,1.f,0);

				//Random points on a disc
				/*while(point.length() > cam->lensSize) {
					point.x = (2.0 * (float)rand()/(float)RAND_MAX - 1);
					point.y = (2.0 * (float)rand()/(float)RAND_MAX - 1);
				}*/


				int polygonSides = 5;
					
				//Random points in a polygon
				while(!isPointInPolygon(point, polygonSides)) {
					point.x = (2.0 * (float)rand()/(float)RAND_MAX - 1);
					point.y = (2.0 * (float)rand()/(float)RAND_MAX - 1);
				}

				point *= cam->lensSize/cam->fNumber;

				ray = cam->eyeRay(i,j,img->width(), img->height());

				Vector3 crossproduct = cross(ray.d, referenceRay.d);
				float dotProduct = dot(ray.d, cam->viewDir());
				float focusDistance = 1/dotProduct * cam->focusDistance;

				ray.o += cam->up() * point.x + cross(cam->up(), -cam->viewDir()).normalize()*point.y;

				ray.d = ((referenceRay.o + referenceRay.d*focusDistance) - ray.o)/ray.d.length();
				ray.d.normalize();

				if (trace(hitInfo, ray))
				{
					shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth));
				}
				else {
					//Image based
					shadeResult += getHDRColorFromVector(ray.d);
				}
			}

			shadeResult = shadeResult/camRays;
			/* TONE MAPPING */
			
			// Film response
			/*float c = 2.0f;
			
			shadeResult = Vector3(1.0) - exp(-c*shadeResult);


			// Gamma correction
			float g = 0.1f;
			shadeResult = shadeResult^(1/g);
						
						*/
			img->setPixel(i, j, shadeResult);
			shadeResult = 0;
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}

	printf("Rendering Progress: 100.000%\n");

	debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
