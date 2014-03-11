#include "MarkovChain.h"
#include "Utils.h"


MarkovChain::MarkovChain() {
    pos = 0;
    count = 0;
    u.resize(states);
    for(int i = 0; i < states; ++i) {
        u[i] = rnd();
    }
}

//double
//MarkovChain::get(int i) const {
//    return u.at(i);
//}

void
MarkovChain::reset() {
    count = 0;
    pos = 0;
}

// TODO: Find ud af om det er et problem hvis den wrapper
double
MarkovChain::getNext() const {
    double ret = u.at(pos);
    count++;
    pos = ( pos + 1 ) % u.size();
    return ret;
}

MarkovChain::MarkovChain(int width, int height) {
    pos = 0;
    count = 0;
    imageWidth = width;
    imageHeight = height;
    u.resize(states);
    for(int i = 0; i < states; ++i) {
        u[i] = rnd();
    }
}

MarkovChain
MarkovChain::large_step(int width, int height) const {
    MarkovChain Result(width, height);
    for(int i = 0; i < states; ++i) {
        Result.u[i] = rnd();
    }
    return Result;
}

MarkovChain MarkovChain::mutate(int width, int height) const {
    MarkovChain Result(width, height);
    Result.u[0] = (perturb(u[0], 2.0 / double(width + height), 0.1));
    Result.u[1] = (perturb(u[1], 2.0 / double(width + height), 0.1));

    for(int i = 2; i < states; ++i) {
        //TODO: Hvad er 1024 og 64  til? 
        Result.u[i] = perturb(u.at(i), 1.0 / 1024.0, 1.0 / 64.0);
    }
    return Result;
}

void prdns::InitRandomNumbersByChain(const MarkovChain MC) {
    u.resize(states);
    for (int i = 0; i < states; i++){
        u[i] = MC.u[i];
    }
}

void prdns::InitRandomNumbers() {
    u.resize(states);
    for (int i = 0; i < states; i++) {
        u[i] = rnd();
    }
}

