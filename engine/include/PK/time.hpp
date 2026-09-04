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

    template <typename U, unsigned short L = 1> class Tracker {
        short ptr{-1}; U records[L]{};
        public:
            void begin(U offset = 0) {
                if (ptr + 1 >= L) {
                    ptr = L - 1;
                    records[ptr] = now<U>() + offset;
                    return;
                }
                records[++ptr] = now<U>() + offset;
            }

            U stop() {
                if (ptr < 0) return U{};
                U elapsed = now<U>() - records[ptr];
                --ptr;
                return elapsed;
            }

            [[nodiscard]] U elapsed() const {
                if (ptr < 0) return U{};
                return now<U>() - records[ptr];
            }

            [[nodiscard]] U delta() {
                if (ptr < 0) return U{};
                U dt = elapsed();
                records[ptr] += dt;
                return dt;
            }
    };
}