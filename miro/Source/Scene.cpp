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
const int pathSamples = Constants::PathSamples;

/*
TODO:	pathTraceFromRay producerer ikke HELT korrekt resultater. Der kommer et 
		par hvide pixels rundt omkring(Tjek loggen på docs). Det kommer nok fra, 
		hvordan fladerne bliver shadet, så der må være et sted hvor der ikke 
		sker det samme.

		1. pathTraceFromRay skal fixes så den producerer et perfekt resultat.
		2. Den skal udvides/modificere med MLT, så den kan perturbere paths. 
        pertube - der skal vi ændre pixel-positionen en smule eller ændre path en bitte smule - hvilken?
		3. Vi skal have tilføjet muligheden for at MLT kan opdatere billedet 
		   direkte, selvom jeg ikke er meget for den øgede coupling :p	
*/

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
    //return Vector3(0,0,0);
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

void Scene::preCalc() {
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
    Vector3 shadeResult = Vector3(0.0f);//pathTraceShading(ray);
    img->setPixel(i, j, shadeResult);
}

void
	Scene::raytraceImage(Camera *cam, Image *img)
{
	int pfmWidth = 1500, pfmHeight = 1500;
	pfmImage = readPFMImage("hdr/stpeters_probe.pfm", &pfmWidth, &pfmWidth);
    
	/*BasicShader basicShader(*this, img, cam);
	basicShader.run();*/
	//PathTracer pathTracer(*this, img, cam, pathSamples);
	//pathTracer.run();
	BiPathTracer biPathTracer(*this, img, cam, pathSamples);
	biPathTracer.run();
	Constants::seconds = 300;
	//MLT mlt(*this, img, cam, pathSamples, &biPathTracer);
	//mlt.run();

	printf("Rendering Progress: 100.000%%\n");
	debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
