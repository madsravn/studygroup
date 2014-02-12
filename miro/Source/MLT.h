#pragma once
#include "utils.h"
#include "Scene.h"

const int MaxEvents = 100;

struct Path {
	HitInfo hits[MaxEvents];	// Points in path
	int size; 					// Length of path
	Path(){
		size=0;
	};

	void add(HitInfo hit) {
		hits[size] = hit;
		size++;
	};
};

inline std::ostream & operator<<(std::ostream& out, const Path& p) {
	out << "Path:" << std::endl;
	for (int i = 0; i < p.size; i++) {
		out << "[" << i << "] = " << p.hits[i].P << ", " << p.hits[i].N << std::endl;
	}
	return out;
}

class MLT
{
public:	
	MLT(Scene& scene) : scene(scene) {};
	~MLT(void) {};
	float mutate(float value);											
	void tracePath(Path& path, const Ray &ray, int recDepth, bool log);
	Path generateEyePath(Ray& eyeRay);
private:
	Scene& scene;
};

