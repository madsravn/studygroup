#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "PFMLoader.h"
#include <iostream>
#include <thread>

Scene * g_scene = 0;

const int recDepth = 3;
const int pathBounces = 3;
const int pathSamples = 8;

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
			//shadeResult = basicShading(ray);
			shadeResult = pathTraceShading(ray);

			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}

	// Add path tracing
	// loop over all pixels in the image
	/*int samples = 8;
	for (int sample = 0; sample < samples; ++sample) {
		for (int j = 0; j < img->height(); ++j)
		{
			for (int i = 0; i < img->width(); ++i)
			{
				ray = cam->eyeRay(i, j, img->width(), img->height());								
				shadeResult = pathTraceShading(ray);
				
				Vector3 pixelColor = img->getPixel(i, j);
				Vector3 newColor = (pixelColor + shadeResult) * (1.0f/(sample+2));
				img->setPixel(i, j, newColor);
			}
			img->drawScanline(j);
			glFinish();
			printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
			fflush(stdout);
		}
	}*/

	printf("Rendering Progress: 100.000%\n");

	debug("done Raytracing!\n");
}

double rnd(void) { 
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123; 
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w; 
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0 / 4294967296.0); 
}

// Based on Toshiyas smallpsmlt
// theory: http://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf  
Vector3 generateRandomRayDirection(Vector3 normal){
	float rand1 = rnd();
	float rand2 = rnd();

	const float temp1 = 2.0 * PI * rand1;
	const float temp2 = pow(rand2, 1.0f / (rand1 + 1.0f));
	const float s = sin(temp1);
	const float c = cos(temp1);
	const float t = sqrt(1.0 - temp2 * temp2);

	Vector3 rayDirection = Vector3(s*t, temp2, c*t);
	
	rayDirection.normalize();
	if(dot(rayDirection, normal) < 0) 
		rayDirection = -rayDirection;

	return rayDirection;
}

Vector3 Scene::tracePath(const Ray ray, int recDepth) {
	if (recDepth > pathBounces) {
		return 0;
	}

	HitInfo hitInfo;
	Vector3 shadeResult = 0;

	if (trace(hitInfo, ray)) {
		shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth));

		// Bounce
		// Generate random ray			// TODO: Extract this to separate function
		Vector3 direction = generateRandomRayDirection(hitInfo.N);
		Ray randomRay = Ray(hitInfo.P, direction);

		// Trace new ray
		Vector3 traceResult = tracePath(randomRay, recDepth + 1);

		shadeResult += traceResult * 0.5f;
	} else {
		shadeResult += getHDRColorFromVector(ray.d);
	}

	return shadeResult;
}

Vector3 Scene::pathTraceShading(const Ray ray) {
	HitInfo hitInfo;
	Vector3 shadeResult = 0;
	

	if (trace(hitInfo, ray)) {
		// First hit shading
		// shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth)) * .5f;

		Vector3 traceResult = 0;

		for (int i = 0; i < pathSamples; ++i) {
			
			// Generate random ray
			Vector3 direction = generateRandomRayDirection(hitInfo.N);
			Ray randomRay = Ray(hitInfo.P, direction);

			// Trace new ray
			traceResult += tracePath(randomRay, 0);	

			
			/*if(traceResult.length() > 1.41421f)
				std::cout << "Final color out of bounds: " << traceResult[0] << traceResult[1] << traceResult[2] << std::endl;
		*/}
		shadeResult += traceResult * (1.0f / (pathSamples + 1));
	} else {
		shadeResult += getHDRColorFromVector(ray.d);
	}

	return shadeResult;	
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
