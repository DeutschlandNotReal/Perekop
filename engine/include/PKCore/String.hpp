#pragma once
#include <PKCore/memory.hpp>
#include <cctype>
#include <cstdlib>

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
        using cchar = const char;
        cchar *data{nullptr}, *cap{0};

        public:
            refstring() = default;
            refstring(const string& str): data(str.begin()), cap(data + str.size()) {}
            refstring(cchar* str): data(str), cap(data + strlen(str)) {}

            template <uint32_t L> refstring(cchar (&str)[L]): data(str), cap(data + L-1) {}

            operator cchar*() const { return data; }
            cchar operator[](uint32_t i) const { return data[i]; } 
            uint32_t size() const { return cap - data; }

            char back() { return *(cap-1); }
            char front() { return *data; }
            cchar* begin() const { return data; }
            cchar* end()   const { return cap; }
            
            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator !() const { return data == nullptr; }

            void next_line(bool* found = nullptr) {
                cchar* ptr = (cchar*)std::memchr(data, '\n', size());
                if (found) *found = (ptr != nullptr);
                if (ptr) data = ptr;
            }

            void skip_space() {
                while (data != cap && isspace(*data)) ++data;
            }

            void adv()        { ++data; }
            void adv(int d)   { data += d; }
            char next()       { return *++data; }
            char consume()    { return *data++; }
            char peek() const { return *data; } // same as front

            float next_float() {
                return strtof(data, (char**)&data);
            }

            uint64_t next_long() {
                return strtoll(data, (char**)&data, 10);
            }
    };
}