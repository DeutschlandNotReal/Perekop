#pragma once
#include <chrono>
#include <thread>
#include <iostream>

namespace pk {
    template <typename T, short L> struct StackTimer {
        private:
            short ptr = -1;
            T records[L];
        public:
            static T now() noexcept { 
                return std::chrono::duration<T>(std::chrono::steady_clock::now().time_since_epoch()).count();
            }

            static void sleep(T duration) {
                if (duration > T(0)) {
                    std::this_thread::sleep_for(std::chrono::duration<T>(duration));
                } else { std::this_thread::yield(); }
            }

            void push() noexcept {
                if (ptr != L - 1) records[++ptr] = now();
            }

            [[nodiscard]] T pop() noexcept {
                return (ptr > -1) ? now() - records[ptr--] : T(0);
            }

            [[nodiscard]] T peek() const noexcept {
                return (ptr > -1) ? records[ptr] : T(0);
            }

            void pop(T& val) {
                val = (ptr > -1) ? now() - records[ptr--]: T(0);
            }

            T elapsed() { return now() - peek(); }

            void pop_message(const std::string& title) {
                T time = pop();
                std::cout << title << ": " << time * T(1000.f) << "ms\n";
            }

            static T delta(T& prev) {
                T dt = now() - prev;
                prev += dt;
                return dt;
            }
    };
}