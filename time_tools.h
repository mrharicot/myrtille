#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <chrono>

struct Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    Timer() { start_time = std::chrono::high_resolution_clock::now(); }

    inline double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
    }

    //inline void reset() const { start_time = std::chrono::high_resolution_clock::now(); }


};

#endif // TIME_TOOLS_H
