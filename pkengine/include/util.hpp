#pragma once
#include <chrono>

namespace pk::util {
    struct timer {
        private:
            int ptr = -1;
            int end;
            double* records;
        public:
            double now() const { 
                return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
            }
            timer() = delete;
            timer(int capacity): end(capacity-1) {
                records = new double[capacity];
            }
            ~timer() { delete[] records; }

            void begin() {
                if (ptr != end) records[++ptr] = now();
            }

            double stop() {
                return (ptr > -1) ? now() - records[ptr--] : 0;
            }
            
            timer(const timer&) = delete;
            timer& operator=(const timer&) = delete;
    };
}