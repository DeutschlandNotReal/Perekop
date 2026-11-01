#pragma once
#include <chrono>
#include <thread>

namespace pk::time {
    template <typename U> U Now() {
        return std::chrono::duration<U>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    template <typename U> void Sleep(U time) {
        std::this_thread::sleep_for(std::chrono::duration<U>(time));
    }

    template <typename U, unsigned short L = 1> class Tracker {
        short ptr{-1}; U records[L]{};
        public:
            void Begin(U offset = 0) noexcept {
                if (ptr + 1 >= L) {
                    ptr = L - 1;
                    records[ptr] = Now<U>() + offset;
                    return;
                }
                records[++ptr] = Now<U>() + offset;
            }

            U Stop() noexcept {
                if (ptr < 0) return U{};
                U elapsed = Now<U>() - records[ptr];
                --ptr;
                return elapsed;
            }

            [[nodiscard]] U Elapsed() const noexcept {
                if (ptr < 0) return U{};
                return Now<U>() - records[ptr];
            }

            U Delta() noexcept {
                if (ptr < 0) return U{};
                U dt = Elapsed();
                records[ptr] += dt;
                return dt;
            }

            void Roll(U delta) noexcept {
                records[ptr] += delta;
            }
    };
}