#pragma once
#include <PKCore/Memory.hpp>

namespace pk {
    class string {
        char* data{nullptr};
        uint32_t len{0};

        public:
            char& operator[](uint32_t i) const { return data[i]; }
            uint32_t size() const { return len; }
            char* begin() const { return data; }
            char* end()   const { return data + len; }
            [[nodiscard]] string() = default;
            [[nodiscard]] string(uint32_t L): data(PKAlloc<char>(L+1)), len(L) { data[L] = '\0'; }

            template <uint32_t L> 
            [[nodiscard]] string(const char (&str)[L]): data(PKAlloc(L)), len(L-1) { 
                PKCopy(data, str, L);
            }

            [[nodiscard]] string(const char* str) {
                len = strlen(str);
                data = PKAlloc(len+1);
                PKCopy(data, str, len+1);
            }

            [[nodiscard]] string(string&& str) {
                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
            }

            string& operator=(string&& str) {
                if (&str == this) return *this;
                if (data) PKFree(data);

                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
                return *this;
            }

            [[nodiscard]] string(const string& str) {
                data = PKAlloc(str.len+1);
                PKCopy(data, str.data, str.len+1);
                len = str.len;
            }

            string& operator=(const string& str) {
                if (&str == this) return *this;
                if (data) {
                    if (str.len != len) { PKFree(data); data = PKAlloc(str.len + 1); }
                } else {
                    data = PKAlloc(str.len + 1);
                }

                PKCopy(data, str.data, str.len+1);
                len = str.len;
                return *this;
            }

            operator char*() const { return data; }

            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator !() const { return data == nullptr; }

            ~string() { PKFree(data); }  
    };

    // referance to string without alloc
    class stringview {
        friend string;
        const char* data{nullptr}; uint32_t len;

        public:
            [[nodiscard]] stringview() = default;
            [[nodiscard]] stringview(const string& str): data(str.begin()), len(str.size()) {}
            [[nodiscard]] stringview(const char* str): data(str), len(strlen(str)) {}

            template <uint32_t L> 
            [[nodiscard]] stringview(char (&str)[L]): data(str), len(L-1) {}

            operator const char*() const { return data; }
            char operator[](uint32_t i) const { return data[i]; } 
            uint32_t size() const { return len; }
            const char* begin() const { return data; }
            const char* end()   const { return data + len; }
            
            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator !() const { return data == nullptr; }
    };
}