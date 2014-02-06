#include "assignment1.h"
#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Sphere.h"

#include "HDRMaterial.h"
#include "PointLight.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "ReflectionMaterial.h"
#include "RefractionMaterial.h"


// local helper function declarations
namespace
{
void addMeshTrianglesToScene(TriangleMesh * mesh, Material * material);
inline Matrix4x4 translate(float x, float y, float z);
inline Matrix4x4 scale(float x, float y, float z);
inline Matrix4x4 rotate(float angle, float x, float y, float z);
} // namespace


void makeCornellBox() {
	g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;
	Triangle* t;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.3f));
    g_camera->setEye(Vector3(0, 1, 3));
    g_camera->setLookAt(Vector3(0, 1, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);
	g_camera->lensSize = 0.001f;
	g_camera->focusDistance = 4; 
	g_camera->aperture = 0.01;
	g_camera->focalLength = 0.04;
	g_camera->fNumber = 3.5;

	Matrix4x4 xform;
    Matrix4x4 xform2;
	Material* material;

	// create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(0, 1.8, -.5));
    light->setColor(Vector3(.73, 0.596, 0.357));
	light->setColor(Vector3(1.0f));
    light->setWattage(2);
    light->setRadius(0.1f);
    g_scene->addLight(light);

		// create and place a point light source
 //   light = new PointLight;
 //   light->setPosition(Vector3(.5, .5, 1));
 //   light->setColor(Vector3(.73, 0.596, 0.357));
	////light->setColor(Vector3(1.0f));
 //   light->setWattage(2);
 //   light->setRadius(0.02f);
 //   g_scene->addLight(light);


	// create the floor triangles
    TriangleMesh * floor1 = new TriangleMesh;
    floor1->createSingleTriangle();
    floor1->setV1(Vector3(-1, 0, 1));
    floor1->setV2(Vector3( 1, 0, 1));
    floor1->setV3(Vector3( 1, 0,-1));
    floor1->setN1(Vector3( 0, 1, 0));
    floor1->setN2(Vector3( 0, 1, 0));
    floor1->setN3(Vector3( 0, 1, 0));
    
	TriangleMesh * floor2 = new TriangleMesh;
    floor2->createSingleTriangle();    
    floor2->setV1(Vector3( 1, 0,-1));
    floor2->setV2(Vector3(-1, 0,-1));
	floor2->setV3(Vector3(-1, 0, 1));
    floor2->setN1(Vector3(0, 1, 0));
    floor2->setN2(Vector3(0, 1, 0));
    floor2->setN3(Vector3(0, 1, 0));
	
	material = new Lambert(Vector3(0,0.5f,0.5f));
	t = new Triangle;
    t->setMesh(floor1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(floor2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the ceiling triangles
    TriangleMesh * ceiling1 = new TriangleMesh;
    ceiling1->createSingleTriangle();
    ceiling1->setV1(Vector3(-1, 2, 1));
    ceiling1->setV2(Vector3(-1, 2,-1));
    ceiling1->setV3(Vector3( 1, 2,-1));
    ceiling1->setN1(Vector3(0, -1, 0));
    ceiling1->setN2(Vector3(0, -1, 0));
    ceiling1->setN3(Vector3(0, -1, 0));
    
	TriangleMesh * ceiling2 = new TriangleMesh;
    ceiling2->createSingleTriangle();    
    ceiling2->setV1(Vector3( 1, 2,-1));
    ceiling2->setV2(Vector3( 1, 2, 1));
	ceiling2->setV3(Vector3(-1, 2, 1));
    ceiling2->setN1(Vector3(0, -1, 0));
    ceiling2->setN2(Vector3(0, -1, 0));
    ceiling2->setN3(Vector3(0, -1, 0));

	material = new Lambert(Vector3(0.5f,0,0.5f));
	t = new Triangle;
    t->setMesh(ceiling1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(ceiling2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the backwall triangles
    TriangleMesh * backwall1 = new TriangleMesh;
    backwall1->createSingleTriangle();
    backwall1->setV1(Vector3(-1, 0,-1));
    backwall1->setV2(Vector3( 1, 0,-1));
    backwall1->setV3(Vector3( 1, 2,-1));
    backwall1->setN1(Vector3(0, 0, 1));
    backwall1->setN2(Vector3(0, 0, 1));
    backwall1->setN3(Vector3(0, 0, 1));
    
	TriangleMesh * backwall2 = new TriangleMesh;
    backwall2->createSingleTriangle();    
    backwall2->setV1(Vector3( 1, 2,-1));
    backwall2->setV2(Vector3(-1, 2,-1));
	backwall2->setV3(Vector3(-1, 0,-1));
    backwall2->setN1(Vector3( 0, 0, 1));
    backwall2->setN2(Vector3( 0, 0, 1));
    backwall2->setN3(Vector3( 0, 0, 1));

	material = new Lambert(Vector3(0.5f,0.5f,0));
	t = new Triangle;
    t->setMesh(backwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(backwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the frontwall triangles
    /*TriangleMesh * frontwall1 = new TriangleMesh;
    frontwall1->createSingleTriangle();
    frontwall1->setV1(Vector3(-1, 0, 1));
    frontwall1->setV2(Vector3( 1, 0, 1));
    frontwall1->setV3(Vector3( 1, 2, 1));
    frontwall1->setN1(Vector3(0, 0, -1));
    frontwall1->setN2(Vector3(0, 0, -1));
    frontwall1->setN3(Vector3(0, 0, -1));
    
	TriangleMesh * frontwall2 = new TriangleMesh;
    frontwall2->createSingleTriangle();    
    frontwall2->setV1(Vector3( 1, 2, 1));
    frontwall2->setV2(Vector3(-1, 2, 1));
	frontwall2->setV3(Vector3(-1, 0, 1));
    frontwall2->setN1(Vector3( 0, 0, -1));
    frontwall2->setN2(Vector3( 0, 0, -1));
    frontwall2->setN3(Vector3( 0, 0, -1));

	material = new Lambert();
	t = new Triangle;
    t->setMesh(frontwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(frontwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);*/

	// create the rightwall triangles
    TriangleMesh * rightwall1 = new TriangleMesh;
    rightwall1->createSingleTriangle();
    rightwall1->setV1(Vector3( 1, 0,-1));
    rightwall1->setV2(Vector3( 1, 0, 1));
    rightwall1->setV3(Vector3( 1, 2, 1));
    rightwall1->setN1(Vector3(-1, 0, 0));
    rightwall1->setN2(Vector3(-1, 0, 0));
    rightwall1->setN3(Vector3(-1, 0, 0));
    
	TriangleMesh * rightwall2 = new TriangleMesh;
    rightwall2->createSingleTriangle();    
    rightwall2->setV1(Vector3( 1, 2, 1));
    rightwall2->setV2(Vector3( 1, 2,-1));
	rightwall2->setV3(Vector3( 1, 0,-1));
    rightwall2->setN1(Vector3(-1, 0, 0));
    rightwall2->setN2(Vector3(-1, 0, 0));
    rightwall2->setN3(Vector3(-1, 0, 0));

	material = new Lambert(Vector3(0,0.5f,0));
	t = new Triangle;
    t->setMesh(rightwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(rightwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the leftwall triangles
    TriangleMesh * leftwall1 = new TriangleMesh;
    leftwall1->createSingleTriangle();
    leftwall1->setV1(Vector3(-1, 0, 1));
    leftwall1->setV2(Vector3(-1, 0,-1));
    leftwall1->setV3(Vector3(-1, 2,-1));
    leftwall1->setN1(Vector3( 1, 0, 0));
    leftwall1->setN2(Vector3( 1, 0, 0));
    leftwall1->setN3(Vector3( 1, 0, 0));
    
	TriangleMesh * leftwall2 = new TriangleMesh;
    leftwall2->createSingleTriangle();    
    leftwall2->setV1(Vector3(-1, 2,-1));
    leftwall2->setV2(Vector3(-1, 2, 1));
	leftwall2->setV3(Vector3(-1, 0, 1));
    leftwall2->setN1(Vector3( 1, 0, 0));
    leftwall2->setN2(Vector3( 1, 0, 0));
    leftwall2->setN3(Vector3( 1, 0, 0));

	material = new Lambert(Vector3(0.5f, 0, 0));
	t = new Triangle;
    t->setMesh(leftwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(leftwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	/*
	TriangleMesh * mesh;
    xform.setIdentity();
	xform *= scale(.2, .2, .2);
	xform *= rotate(30, 0, 1, 0);
    xform *= translate(-1, 0, 3);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(0, 0, 0.5f));
    addMeshTrianglesToScene(mesh, material);
	
	xform.setIdentity();
	xform *= scale(.4, .4, .4);
    xform *= translate(.5, 1.0, -.5);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new RefractionMaterial(1.31f);
    addMeshTrianglesToScene(mesh, material);

	xform.setIdentity();
	xform *= scale(.3, .3, .3);
    xform *= translate(-2, 4, 2);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new ReflectionMaterial();
    addMeshTrianglesToScene(mesh, material);
	*/

	/*
    xform.setIdentity();
	xform *= scale(.3, .3, .3);
    xform *= translate(1.5, -.5, 1);
	material = new Lambert(Vector3(128, 255, 128));
    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("mesh\\bunny.obj", xform);
    addMeshTrianglesToScene(bunny, material);
	*/

	// let objects do pre-calculations if needed
    g_scene->preCalc();
}


void makeCornellBox2() {
	g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;
	Triangle* t;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.3f));
    g_camera->setEye(Vector3(0, 1, 3));
    g_camera->setLookAt(Vector3(0, 1, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);
	g_camera->lensSize = 0.001f;
	g_camera->focusDistance = 4; 
	g_camera->aperture = 0.01;
	g_camera->focalLength = 0.04;
	g_camera->fNumber = 3.5;

	Matrix4x4 xform;
    Matrix4x4 xform2;
	Material* material;

	// create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(0, 1.8, -.5));
    light->setColor(Vector3(.73, 0.596, 0.357));
	light->setColor(Vector3(1.0f));
    light->setWattage(2);
    light->setRadius(0.1f);
    g_scene->addLight(light);

/*		// create and place a point light source
    light = new PointLight;
    light->setPosition(Vector3(.5, .5, 1));
    light->setColor(Vector3(.73, 0.596, 0.357));
	//light->setColor(Vector3(1.0f));
    light->setWattage(2);
    light->setRadius(0.02f);
    g_scene->addLight(light);*/


	// create the floor triangles
    TriangleMesh * floor1 = new TriangleMesh;
    floor1->createSingleTriangle();
    floor1->setV1(Vector3(-1, 0, 1));
    floor1->setV2(Vector3( 1, 0, 1));
    floor1->setV3(Vector3( 1, 0,-1));
    floor1->setN1(Vector3( 0, 1, 0));
    floor1->setN2(Vector3( 0, 1, 0));
    floor1->setN3(Vector3( 0, 1, 0));
    
	TriangleMesh * floor2 = new TriangleMesh;
    floor2->createSingleTriangle();    
    floor2->setV1(Vector3( 1, 0,-1));
    floor2->setV2(Vector3(-1, 0,-1));
	floor2->setV3(Vector3(-1, 0, 1));
    floor2->setN1(Vector3(0, 1, 0));
    floor2->setN2(Vector3(0, 1, 0));
    floor2->setN3(Vector3(0, 1, 0));
	
	material = new Lambert(Vector3(0.2f,0.1f,0.6f));
	t = new Triangle;
    t->setMesh(floor1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(floor2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the ceiling triangles
    TriangleMesh * ceiling1 = new TriangleMesh;
    ceiling1->createSingleTriangle();
    ceiling1->setV1(Vector3(-1, 2, 1));
    ceiling1->setV2(Vector3(-1, 2,-1));
    ceiling1->setV3(Vector3( 1, 2,-1));
    ceiling1->setN1(Vector3(0, -1, 0));
    ceiling1->setN2(Vector3(0, -1, 0));
    ceiling1->setN3(Vector3(0, -1, 0));
    
	TriangleMesh * ceiling2 = new TriangleMesh;
    ceiling2->createSingleTriangle();    
    ceiling2->setV1(Vector3( 1, 2,-1));
    ceiling2->setV2(Vector3( 1, 2, 1));
	ceiling2->setV3(Vector3(-1, 2, 1));
    ceiling2->setN1(Vector3(0, -1, 0));
    ceiling2->setN2(Vector3(0, -1, 0));
    ceiling2->setN3(Vector3(0, -1, 0));

	material = new Lambert(Vector3(0.7f, 0.3, 0.65f));
	t = new Triangle;
    t->setMesh(ceiling1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(ceiling2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the backwall triangles
    TriangleMesh * backwall1 = new TriangleMesh;
    backwall1->createSingleTriangle();
    backwall1->setV1(Vector3(-1, 0,-1));
    backwall1->setV2(Vector3( 1, 0,-1));
    backwall1->setV3(Vector3( 1, 2,-1));
    backwall1->setN1(Vector3(0, 0, 1));
    backwall1->setN2(Vector3(0, 0, 1));
    backwall1->setN3(Vector3(0, 0, 1));
    
	TriangleMesh * backwall2 = new TriangleMesh;
    backwall2->createSingleTriangle();    
    backwall2->setV1(Vector3( 1, 2,-1));
    backwall2->setV2(Vector3(-1, 2,-1));
	backwall2->setV3(Vector3(-1, 0,-1));
    backwall2->setN1(Vector3( 0, 0, 1));
    backwall2->setN2(Vector3( 0, 0, 1));
    backwall2->setN3(Vector3( 0, 0, 1));

	material = new Lambert(Vector3(0.9f,0.5f,0.3f));
	t = new Triangle;
    t->setMesh(backwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(backwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the rightwall triangles
    TriangleMesh * rightwall1 = new TriangleMesh;
    rightwall1->createSingleTriangle();
    rightwall1->setV1(Vector3( 1, 0,-1));
    rightwall1->setV2(Vector3( 1, 0, 1));
    rightwall1->setV3(Vector3( 1, 2, 1));
    rightwall1->setN1(Vector3(-1, 0, 0));
    rightwall1->setN2(Vector3(-1, 0, 0));
    rightwall1->setN3(Vector3(-1, 0, 0));
    
	TriangleMesh * rightwall2 = new TriangleMesh;
    rightwall2->createSingleTriangle();    
    rightwall2->setV1(Vector3( 1, 2, 1));
    rightwall2->setV2(Vector3( 1, 2,-1));
	rightwall2->setV3(Vector3( 1, 0,-1));
    rightwall2->setN1(Vector3(-1, 0, 0));
    rightwall2->setN2(Vector3(-1, 0, 0));
    rightwall2->setN3(Vector3(-1, 0, 0));

	material = new Lambert(Vector3(0.1f,0.5f,0.1f));
	t = new Triangle;
    t->setMesh(rightwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(rightwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);

	// create the leftwall triangles
    TriangleMesh * leftwall1 = new TriangleMesh;
    leftwall1->createSingleTriangle();
    leftwall1->setV1(Vector3(-1, 0, 1));
    leftwall1->setV2(Vector3(-1, 0,-1));
    leftwall1->setV3(Vector3(-1, 2,-1));
    leftwall1->setN1(Vector3( 1, 0, 0));
    leftwall1->setN2(Vector3( 1, 0, 0));
    leftwall1->setN3(Vector3( 1, 0, 0));
    
	TriangleMesh * leftwall2 = new TriangleMesh;
    leftwall2->createSingleTriangle();    
    leftwall2->setV1(Vector3(-1, 2,-1));
    leftwall2->setV2(Vector3(-1, 2, 1));
	leftwall2->setV3(Vector3(-1, 0, 1));
    leftwall2->setN1(Vector3( 1, 0, 0));
    leftwall2->setN2(Vector3( 1, 0, 0));
    leftwall2->setN3(Vector3( 1, 0, 0));

	material = new Lambert(Vector3(0.8f, 0.1f, 0.1f));
	t = new Triangle;
    t->setMesh(leftwall1);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	t = new Triangle;
    t->setMesh(leftwall2);
    t->setMaterial(material); 
    g_scene->addObject(t);
	
	
    // teapot
	TriangleMesh * mesh;
    xform.setIdentity();
	xform *= scale(.2, .2, .2);
	xform *= rotate(30, 0, 1, 0);
    xform *= translate(-1, 0, 3);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(0, 0, 0.5f));
    addMeshTrianglesToScene(mesh, material);
	

    // Sphere
	xform.setIdentity();
	xform *= scale(.4, .4, .4);
    xform *= translate(.5, 1.0, -.5);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new RefractionMaterial(1.31f);
    addMeshTrianglesToScene(mesh, material);

    // Sphere
	xform.setIdentity();
	xform *= scale(.3, .3, .3);
    xform *= translate(-2, 4, 1);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new ReflectionMaterial();
    addMeshTrianglesToScene(mesh, material);	

	/*
    xform.setIdentity();
	xform *= scale(.3, .3, .3);
    xform *= translate(1.5, -.5, 1);
	material = new Lambert(Vector3(128, 255, 128));
    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("mesh\\bunny.obj", xform);
    addMeshTrianglesToScene(bunny, material);
	*/

	// let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeTeapotScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.3f));
    g_camera->setEye(Vector3(0, 3, 6));
    g_camera->setLookAt(Vector3(0, .5, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);
	g_camera->lensSize = 0.001f;
	g_camera->focusDistance = 4; 
	g_camera->aperture = 0.01;
	g_camera->focalLength = 0.04;
	g_camera->fNumber = 3.5;	

	
    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(10, 10, 10));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(700);
    g_scene->addLight(light);

	// create and place a point light source
    light = new PointLight;
    light->setPosition(Vector3(-4, 7, 10));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(700);
    g_scene->addLight(light);

	// create and place a point light source
    light = new PointLight;
    light->setPosition(Vector3(0, 8, -7));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(700);
    g_scene->addLight(light);
    
	Matrix4x4 xform;
    Matrix4x4 xform2;
	Material* material;
	
	TriangleMesh * mesh;
    xform.setIdentity();
    xform *= translate(2, 0, 2);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(1, 1, 1));
    addMeshTrianglesToScene(mesh, material);

    xform.setIdentity();
    xform *= translate(-2, 0, 2);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(0,0,1));
    addMeshTrianglesToScene(mesh, material);

	xform.setIdentity();
    xform *= translate(2, 0, -2);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(0,1,0));
    addMeshTrianglesToScene(mesh, material);

	xform.setIdentity();
    xform *= translate(-2, 0, -2);
    mesh = new TriangleMesh;
    mesh->load("mesh/teapot.obj", xform);
	material = new Lambert(Vector3(1, 0, 0));
    addMeshTrianglesToScene(mesh, material);	
	
	xform.setIdentity();
	xform *= scale(1, 1, 1);
    xform *= translate(0, 1, 0);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new RefractionMaterial(0.7f);
    addMeshTrianglesToScene(mesh, material);

	xform.setIdentity();
	xform *= scale(1, 1, 1);
    xform *= translate(0, 3, 0);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new ReflectionMaterial();
    addMeshTrianglesToScene(mesh, material);

	/*xform.setIdentity();
	xform *= scale(1, 1, 1);
    xform *= translate(0, 3, 0);
    mesh = new TriangleMesh;
    mesh->load("mesh/sphere.obj", xform);
	material = new Lambert(1, 0, 0, 300, Vector3(0, 0, 1));
    addMeshTrianglesToScene(mesh, material);*/

	TriangleMesh * plane = new TriangleMesh;	
    plane->createSingleTriangle();
    plane->setV1(Vector3(-10, 0, -10));
	plane->setV2(Vector3(  0, 10, -10));
	plane->setV3(Vector3( 10, 0, -10));
	plane->setN1(Vector3(0, 0, 1));
	plane->setN2(Vector3(0, 0, 1));
	plane->setN3(Vector3(0, 0, 1));
    
	Triangle* t;
	material = new Lambert(Vector3(0, 0, 0));
    t = new Triangle;
    t->setIndex(0);
    t->setMesh(plane);
    t->setMaterial(material); 
    g_scene->addObject(t);

    // create the floor triangle
    TriangleMesh * floor = new TriangleMesh;
    floor->createSingleTriangle();
	floor->setV1(Vector3(-10, 0, -10));
	floor->setV2(Vector3(  0, 0,  10));
	floor->setV3(Vector3( 10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));
    
	material = new Lambert();
	t = new Triangle;
    t->setMesh(floor);
    t->setMaterial(material); 
    g_scene->addObject(t);

    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

// local helper function definitions
namespace
{

void
addMeshTrianglesToScene(TriangleMesh * mesh, Material * material)
{
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < mesh->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(mesh);
        t->setMaterial(material); 
        g_scene->addObject(t);
    }
}


inline Matrix4x4
translate(float x, float y, float z)
{
    Matrix4x4 m;
    m.setColumn4(Vector4(x, y, z, 1));
    return m;
}


inline Matrix4x4
scale(float x, float y, float z)
{
    Matrix4x4 m;
    m.m11 = x;
    m.m22 = y;
    m.m33 = z;
    return m;
}

// angle is in degrees
inline Matrix4x4
rotate(float angle, float x, float y, float z)
{
    float rad = angle*(PI/180.);
    
    float x2 = x*x;
    float y2 = y*y;
    float z2 = z*z;
    float c = cos(rad);
    float cinv = 1-c;
    float s = sin(rad);
    float xy = x*y;
    float xz = x*z;
    float yz = y*z;
    float xs = x*s;
    float ys = y*s;
    float zs = z*s;
    float xzcinv = xz*cinv;
    float xycinv = xy*cinv;
    float yzcinv = yz*cinv;
    
    Matrix4x4 m;
    m.set(x2 + c*(1-x2), xy*cinv+zs, xzcinv - ys, 0,
          xycinv - zs, y2 + c*(1-y2), yzcinv + xs, 0,
          xzcinv + ys, yzcinv - xs, z2 + c*(1-z2), 0,
          0, 0, 0, 1);
    return m;
}

} // namespace


