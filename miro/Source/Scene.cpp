#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include "PFMLoader.h"
#include <iostream>
#include <thread>
#include "Utils.h"
#include "PointLight.h"
#include <future>
#include <exception>

Scene * g_scene = 0;
const int recDepth = 5;
const int pathBounces = 5;
const int pathSamples = 4;

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

	//return ret;
    return Vector3(0,0,0);
}

// Can't remember where I found this algorithm, I took it from a project I made two years ago
bool isPointInPolygon(const Vector3 &point, const int polygonSides) {
	Vector3* points = new Vector3[polygonSides];
	for (int i = 0; i < polygonSides; i++) {
		points[i] = Vector3(
			cos(2*(PI/polygonSides) * i),	//Polar to cartesian coordinates
			sin(2*(PI/polygonSides) * i), 0);
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

void Scene::multithread( Ray ray, Camera* cam, Image* img, int i, int j) {      
    ray = cam->eyeRay(i, j, img->width(), img->height());					
    Vector3 shadeResult = pathTraceShading(ray);
    img->setPixel(i, j, shadeResult);
}

void
	Scene::raytraceImage(Camera *cam, Image *img)
{
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	int pfmWidth = 1500, pfmHeight = 1500;
	pfmImage = readPFMImage("hdr/stpeters_probe.pfm", &pfmWidth, &pfmWidth);

#if !defined (_WIN32)
    std::cout << "Multithreaded" << std::endl;
    std::vector<std::future<void>> futures;
    for(int j = 0; j < img->height(); ++j) {
        for(int i = 0; i < img->width(); ++i) {
            futures.push_back(std::async(&Scene::multithread, this, ray, cam, img, i, j));
        }
    }
    auto size = futures.size();
    int progress = 0;
    for(auto &e : futures) {
        try {
            e.get();
            progress++;
            printf("Rendering Progress: %.3f%%\r", progress/float(size)*100.0f);
            fflush(stdout);

        } catch (const std::exception& e) {
            std::cerr << "EXCEPTION: " << e.what() << std::endl;
        }
    }

    for(int j = 0; j < img->height(); ++j) {
        img->drawScanline(j);
    }
    glFinish();

#else

    //OLD STUFF
    
	// loop over all pixels in the image

	//MLT mlt = MLT(*this);
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			
			ray = cam->eyeRay(i, j, img->width(), img->height());	
			
			/*Path path = mlt.generateEyePath(ray);

			if (path.size > 1) {
				std::cout << path << std::endl;
			}*/
			//shadeResult = basicShading(ray);
			shadeResult = pathTraceShading(ray);
			//shadeResult = biPathTraceShading(ray);
			//shadeResult = pathTraceFromPath(path);
			img->setPixel(i, j, shadeResult);
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}
#endif

	printf("Rendering Progress: 100.000%\n");

	debug("done Raytracing!\n");
}

//Vector3 Scene::pathTraceFromPath(Path path) {
//	Vector3 shadeResult = Vector3(0.0f);
//
//	for (int i = 1; i < path.size; i++) { // 0 = eye point	
//		HitInfo hit = path.hits[i];
//	}
//}

Vector3 Scene::tracePath(const Ray &ray, int recDepth, bool log) {

	HitInfo hitInfo;
	Vector3 shadeResult = 0;

    if(log) std::cout << "Ray is " << ray << std::endl;
	if (trace(hitInfo, ray, 0.0001f)) {
		shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth));		
	} 

	return shadeResult;
}

// Unused
Vector3 Scene::biPathTraceShading(const Ray &ray) {
	HitInfo hitInfo;
	Vector3 shadeResult = 0;
	
	HitInfo eyePath[pathBounces];
	// Walk from eye
	if (trace(hitInfo, ray, 0.0001f)) {	
		Vector3 direction = generateRandomRayDirection(hitInfo.N);
		Ray randomRay = Ray(hitInfo.P, direction);
		for(int i = 0; i < pathBounces; ++i) {

			eyePath[i] = hitInfo;

			if (trace(hitInfo, randomRay, 0.0001f)) {

			//TODO: Do something
			}
		}
	} else {

	}

	// Walk from lights
	for (int i = 0; i < g_scene->lights()->size(); ++i) {
		HitInfo lightPath[pathBounces];		

		const PointLight *light = (g_scene->lights())->at(i);

		// First entry is the light's own position				// Maybe find an alternative to this
		lightPath[0] = HitInfo(0.0f, light->position());

		Vector3 direction = generateRandomRayDirection();
		Ray randomRay = Ray(light->position(), direction);

		for(int i = 1; i < pathBounces; ++i) {

			lightPath[i] = hitInfo;

			if (trace(hitInfo, ray, 0.0001f)) {
				
				// TODO: Do something

				Vector3 direction = generateRandomRayDirection(hitInfo.N);
				randomRay = Ray(hitInfo.P, direction);
			}		
		}
	}
	return shadeResult;
}

Vector3 Scene::pathTraceShading(const Ray &ray, bool log) {
	HitInfo hitInfo;
	Vector3 shadeResult = 0;

    if(log) std::cout << "Ray is " << ray << std::endl;		

	float inversePathSamples = 1.0f / (float)(pathSamples);
	for (int i = 0; i < pathSamples; ++i) {	
		// Trace new ray
		if (trace(hitInfo, ray, 0.0001f)) {
			shadeResult += hitInfo.material->shade(ray, hitInfo, *this, 0) * inversePathSamples;		
		} 
	}		

	return shadeResult;
}

Vector3 Scene::basicShading(const Ray &ray) {
	
	HitInfo hitInfo;
	Vector3 shadeResult = 0;
	
	if (trace(hitInfo, ray, 0.0001f))
	{
		shadeResult += (hitInfo.material->shade(ray, hitInfo, *this, recDepth));
	}	

	return shadeResult;
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
