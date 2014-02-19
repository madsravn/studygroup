#ifndef MARKOVCHAIN_H_CSE68
#define MARKOVCHAIN_H_CSE68
#include <vector>

const int states = 128;

class MarkovChain {
    public:
        MarkovChain();
        MarkovChain(int width, int height);
        MarkovChain large_step() const;
        MarkovChain mutate(int width, int height) const;
        void reset();
        double getNext() const;
        std::vector<double> u;
        int imageWidth, imageHeight;
        mutable int pos;
};

class prdns {
    public:
        // internal states of random numbers
        std::vector<double> u;
        void InitRandomNumbersByChain(const MarkovChain MC);
        void InitRandomNumbers();
};



/*
const NumStates = 128;
struct TMarkovChain { std::vector<double> u; //PathContribution C;
	TMarkovChain() {for (int i = 0; i < NumStates; i++) u.push_back(rnd());}
	TMarkovChain large_step() const {
		TMarkovChain Result;
		//Result.C = (*this).C;
		for (int i = 0; i < NumStates; i++) Result.u[i] = rnd();
		return Result;
	}
	TMarkovChain mutate() const {
		TMarkovChain Result;
		//Result.C = (*this).C;

		// pixel location
		Result.u[0] = perturb(u[0], 2.0 / double(PixelWidth+PixelHeight), 0.1);
		Result.u[1] = perturb(u[1], 2.0 / double(PixelWidth+PixelHeight), 0.1);

		// the rest
		for (int i = 2; i < NumStates; i++) Result.u[i] = perturb(u[i], 1.0 / 1024.0, 1.0 / 64.0);
		return Result;
	}
};*/

#endif
