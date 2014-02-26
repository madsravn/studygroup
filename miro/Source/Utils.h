#ifndef UTILS_H_CSE68
#define UTILS_H_CSE68
#include "Vector3.h"
#include <vector>
double rnd(void);  
// Based on Toshiyas smallpsmlt
// theory: http://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf  
Vector3 generateRandomRayDirection(Vector3 normal, float rand1, float rand2);
Vector3 generateRandomRayDirection(float rand1, float rand2);
Vector3 generateRandomRayDirection();
Vector3 generateRandomRayDirection(Vector3 normal);

template <class T> 
std::vector<T> subVector(const std::vector<T> &vector, int start, int end) {
    typename std::vector<T>::const_iterator first = vector.begin() + start;
    typename std::vector<T>::const_iterator last = vector.begin() + end;
	return std::vector<T>(first, last);
};

double maxVectorValue(Vector3 &vector);
double luminance(Vector3 color);
Vector3 clamp(Vector3 vector, float lowerBound, float upperBound);

// primary space Markov chain
inline double perturb(const double value, const double s1, const double s2) {
	double Result;
	double r = rnd();
	if (r < 0.5) {
		r = r * 2.0;
		Result = value + s2 * exp(-log(s2 / s1) * r); if (Result > 1.0) Result -= 1.0;
	} else {
		r = (r - 0.5) * 2.0;
		Result = value - s2 * exp(-log(s2 / s1) * r); if (Result < 0.0) Result += 1.0;
	}
    //std::cout << "Result = " << Result << std::endl;
	return Result;
}



#endif
