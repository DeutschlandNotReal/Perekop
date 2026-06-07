#pragma once
#include <PKCore/memory.hpp>

namespace pk {
    class string {
        char* data{nullptr}; uint32_t len{0};

        void nullterm() { data[len] = '\0'; }
        public:
            char& operator[](uint32_t i) const { return data[i]; }

            uint32_t size() const { return len; }
            char* begin() const { return data; }
            char* end()   const { return data + len; }
            string() = default;
            string(uint32_t L): data(PKAlloc<char>(L + 1)), len(L) { nullterm(); }

            template <uint32_t L> 
            string(const char (&str)[L]): data(PKAlloc<char>(L)), len(L - 1) { 
                PKCopy(data, str, L);
                nullterm();
            }

            string(const char* str) {
                len = strlen(str);
                data = PKAlloc<char>(len+1);
                PKCopy(data, str, len+1);
                nullterm();
            }

            string(string&& str) {
                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
            }

            string& operator=(string&& str) {
                if (&str == this) return *this;
                if (data) PKFree(data);

                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
                nullterm();
                return *this;
            }

            string(const string& str) {
                data = PKAlloc<char>(str.len+1);
                PKCopy(data, str.data, str.len + 1);
                len = str.len;
                nullterm();
            }

            string& operator=(const string& str) {
                if (&str == this) return *this;

                if (data) {
                    if (len != str.len) { PKFree(data); data = PKAlloc<char>(str.len + 1); }
                } else { data = PKAlloc<char>(str.len + 1); }

                PKCopy(data, str.data, str.len);
                len = str.len;
                str.data[len] = '\0';
                nullterm();
                return *this;
            }

            operator char*() const { return data; }

            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator !() const { return data == nullptr; }

            ~string() { if (data) PKFree(data); }  
    };

    // referance to string without owning
    class refstring {
        friend string;
        const char* data{nullptr}; uint32_t len{0};

        public:
            refstring() = default;
            refstring(const string& str): data(str.begin()), len(str.size()) {}
            refstring(const char* str): data(str), len(strlen(str)) {}

            template <uint32_t L> refstring(const char (&str)[L]): data(str), len(L-1) {}

            operator const char*() const { return data; }
            char operator[](uint32_t i) const { return data[i]; } 
            uint32_t size() const { return len; }

            const char* begin() const { return data; }
            const char* end()   const { return data + len; }
            
            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator !() const { return data == nullptr; }
    };
}