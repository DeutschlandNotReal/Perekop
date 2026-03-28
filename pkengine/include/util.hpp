#pragma once
#include <chrono>
#include <thread>

namespace pk::util {
    template <typename T, short L> struct StackTimer {
        private:
            short ptr = -1;
            T records[L];
        public:
            static T now() noexcept { 
                return std::chrono::duration<T>(std::chrono::steady_clock::now().time_since_epoch()).count();
            }

            static void sleep(T duration) {
                std::this_thread::sleep_for(std::chrono::duration<T>(duration));
            }

            void begin() noexcept {
                if (ptr != L - 1) records[++ptr] = now();
            }

            T stop() noexcept {
                return (ptr > -1) ? now() - records[ptr--] : T(0);
            }
    };
}