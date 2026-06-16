#pragma once
#include <chrono>
#include <thread>

namespace pk::time {
    template <typename U> U now() {
        return std::chrono::duration<U>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    template <typename U> void sleep(U time) {
        std::this_thread::sleep_for(std::chrono::duration<U>(time));
    }

    template <typename U> class Stopwatch {
        U* result; U start;
        Stopwatch(U* result): result(result), start(now<U>()) {};
        ~Stopwatch() { *result = now<U>() - start; }
    };

    template <typename U, unsigned short L = 1> class Tracker {
        short ptr{-1}; U records[L];
        public:
            void begin(U offset = 0) { records[++ptr] = now<U>() + offset; }
            U stop() { return now<U>() - records[ptr--]; }
            [[nodiscard]] U elapsed() const { return now<U>() - records[ptr]; }
            [[nodiscard]] U delta() {
                U dt = elapsed(); records[ptr] += dt; return dt;
            }
    };
}