#pragma once
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

class BasicShader
{
public:
	BasicShader(Scene& scene, Image* image, Camera* camera);
	~BasicShader(void);
	void run();
private:
	Scene& scene;
	Image* img;
	Camera* cam;
};