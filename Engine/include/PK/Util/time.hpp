#pragma once
#include <chrono>
#include <thread>

namespace pk::time {
    template <typename U> U now() {
        return std::chrono::duration<U>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    template <typename U> void sleepms(U time) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    template <typename U, unsigned short L = 1> class tracker {
        short ptr{-1}; U records[L]{};
        public:
            void begin(U offset = 0) noexcept {
                if (ptr + 1 >= L) {
                    ptr = L - 1;
                    records[ptr] = now<U>() + offset;
                    return;
                }
                records[++ptr] = now<U>() + offset;
            }

            U stop() noexcept {
                if (ptr < 0) return U{};
                U elapsed = now<U>() - records[ptr];
                --ptr;
                return elapsed;
            }

            [[nodiscard]] U elapsed() const noexcept {
                if (ptr < 0) return U{};
                return now<U>() - records[ptr];
            }

            U delta() noexcept {
                if (ptr < 0) return U{};
                U dt = elapsed();
                records[ptr] += dt;
                return dt;
            }

            void roll(U delta) noexcept {
                records[ptr] += delta;
            }
    };
}