#pragma once
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

class PathTracer
{
public:
	PathTracer(Scene& scene, Image* image, Camera* camera, int pathSamples);
	~PathTracer(void);
	void run();
private:
	Scene& scene;
	Image* img;
	Camera* cam;
	int samples;


};

