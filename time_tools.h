#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <chrono>

struct Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    Timer() { start_time = std::chrono::high_resolution_clock::now(); }

    inline double elapsed(bool reset_timer = false) {
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
        if (reset_timer)
            reset();
        return elapsed_time;
    }

    inline void reset() { start_time = std::chrono::high_resolution_clock::now(); }


};

#endif // TIME_TOOLS_H
