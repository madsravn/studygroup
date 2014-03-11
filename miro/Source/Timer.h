#include <chrono>


class Timer {
    private:
        std::chrono::high_resolution_clock::time_point t1, t2;
    public:
        Timer() { }
        void start() { t1 = std::chrono::high_resolution_clock::now(); }
        inline std::chrono::milliseconds duration() { t2 = std::chrono::high_resolution_clock::now(); return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1); }

};

