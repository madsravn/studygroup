#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace Constants {
    const int MinPathLength = 2;
    const int MaxPathLength = 12;
    const int N_INIT = 10000;
    const double LargeStepProb = 0.3;
    const int NumRNGsPerEvent = 2;
    const int MaxEvents = MaxPathLength + 1;
    const int NumStatesSubpath = (MaxEvents + 2) * NumRNGsPerEvent;
    const int NumStates = NumStatesSubpath * 2;
    const int PathSamples = 16;
    extern int seconds;
}
   
#endif
