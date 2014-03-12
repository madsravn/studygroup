#ifndef ITRACER_H_
#define ITRACER_H_
#include "MarkovChain.h"
class ITracer {
    public:
        virtual PathContribution calcPathContribution(const MarkovChain& MC) const = 0;

    protected:
        ITracer() {}

};
#endif
