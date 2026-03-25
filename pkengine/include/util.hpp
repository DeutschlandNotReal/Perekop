#pragma once
#include <vector>
#include <functional>
#include <chrono>

namespace pk::util {
    template <typename T> void swappop(std::vector<T>& V, unsigned int I, std::function<void(T&)> swapf) {
        if ( I != V.size() - 1 ) {
            V[I] = V.back();
            swapf(V[I]);
        }
        V.pop_back();
    };

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

            void push() {
                if (ptr == end) return;
                records[++ptr] = now();
            }

            double pop() {
                if (ptr == -1) return 0;
                return now() - records[ptr--];
            }
 
            double record(std::function<void(void)> f) {
                double start = now();
                f();
                return now() - start;
            }
            
            timer(const timer&) = delete;
            timer& operator=(const timer&) = delete;
    };
}

#undef u16