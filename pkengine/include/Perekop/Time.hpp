#pragma once
#include <chrono>
#include <thread>
#include <iostream>

namespace pk {
    template <typename T, short L> class StackTimer {
        short ptr = -1;
        T records[L];
        public:
            static T now() noexcept { 
                return std::chrono::duration<T>(std::chrono::steady_clock::now().time_since_epoch()).count();
            }

            static void sleep(T duration) {
                if (duration > T(0))
                    std::this_thread::sleep_for(std::chrono::duration<T>(duration));
                else 
                    std::this_thread::yield();
            }

            void push() noexcept {
                if (ptr != L - 1) records[++ptr] = now();
            }

            [[nodiscard]] T pop() noexcept {
                return (ptr > -1) ? now() - records[ptr--] : T(0);
            }

            [[nodiscard]] T top() const noexcept {
                return (ptr > -1) ? records[ptr] : T(0);
            }

            void pop(T& val) {
                val = (ptr > -1) ? now() - records[ptr--]: T(0);
            }

            T elapsed() { return now() - top(); }

            T delta() {
                if (ptr < 0) return T(0);
                T cur = now();
                T dt = cur - records[ptr];
                records[ptr] = cur;
                return dt;
            }

            void pop_ms(const std::string& title) {
                float time = pop();
                int ms = time * 1e3;
                int us = time * 1e6 - ms * 1e3;
                std::cout << title << ": " << ms << '.' << us << "ms\n";
            }
    };
}