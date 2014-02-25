#include "Vector3.h"

class Contribution {
	public:
		double x, y; 		// Pixel coordinate
		Vector3 color; 				// Color
		Contribution(){}; 
		Contribution(double x_, double y_, Vector3 c_) : x(x_), y(y_), color(c_) {}
};