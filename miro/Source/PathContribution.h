#ifndef PATHCONTRIBUTION_H_CSE68
#define PATHCONTRIBUTION_H_CSE68

#include <vector>
#include "Contribution.h"
class PathContribution {
	public:
		std::vector<Contribution> colors;	// Array of contribution elements
		double scalarContribution; 		// Scalar contribution
		PathContribution(){		
			scalarContribution = 0.0;
		};
};

#endif