#pragma once
#include <chrono>
#include <thread>

namespace pk {
    template <typename T, short L> class StackTimer {
        short ptr = -1;
        T records[L];
        public:
            static T now() noexcept { 
                return std::chrono::duration<T>(std::chrono::steady_clock::now().time_since_epoch()).count();
            }

            static void sleep(T duration) {
                std::this_thread::sleep_for(std::chrono::duration<T>(duration));
            }

            void begin() noexcept { records[++ptr] = now(); }
            void begin(T offset) noexcept { records[++ptr] = now() + offset; }
            [[nodiscard]] T stop() noexcept { return now() - records[ptr--]; }
            [[nodiscard]] T elapsed() const noexcept { return now() - records[ptr]; }
            [[nodiscard]] T delta() {
                T dt = elapsed();
                records[ptr] += dt;
                return dt;
            }
    };
}