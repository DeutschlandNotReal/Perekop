#pragma once
#include <PKCore/memory.hpp>

namespace pk {
    class strview;
    class string {
        char* data{nullptr}; uint32_t len{0};

        public:
            char& operator[](uint32_t i) const { return data[i]; }

            uint32_t size() const { return len; }
            char* begin() const { return data; }
            char* end()   const { return data + len; }
            string() = default;
            string(uint32_t L): data(pk::alloc(L + 1)), len(L) { data[L] = '\0'; }
            string(char* data, uint32_t len): data(data), len(len) {} // string assumes ownership

            template <uint32_t L> 
            string(const char (&str)[L]): data(pk::alloc(L)), len(L - 1) { 
                pk::copy(data, str, L);
            }

            string(const char* str) {
                len = strlen(str);
                data = alloc(len+1);
                copy(data, str, len+1);
            }

            string(string &&str) {
                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
            }

            string& operator=(string &&str) {
                if (&str == this) return *this;
                if (data) pk::free(data);

                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
                return *this;
            }

            string(const string &str) {
                data = alloc(str.len+1);
                copy(data, str.data, str.len + 1);
                len = str.len;
            }

            string& operator=(const string& str) {
                if (&str == this) return *this;

                if (!data || len != str.len) {
                    if (!data) pk::free(data);
                    data = pk::alloc(str.len + 1);
                }

                pk::copy(data, str.data, str.len);
                len = str.len;
                return *this;
            }

            operator char*() const { return data; }

            explicit operator bool() const { return data != nullptr; }
            bool operator !()        const { return data == nullptr; }

            bool operator>(const string& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            bool operator<(const string& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            bool operator==(const string& b) const {
                return !(b.len != len || strncmp(data, b.data, len));
            }

            const strview& view() const { 
                // REINTERPRET CAST! must be const
                return *(strview*)this; 
            }

            ~string() { if (data) free(data); }  
    };

    // view string, non owning
    class strview {
        const char* data{nullptr}; uint32_t len{0};

        public:
            strview() = default;
            strview(const string &str): data(str.begin()), len(str.size()) {}
            strview(const char* str): data(str), len(strlen(str)) {}
            strview(const char* str, const char* end): data(str), len(end - data) {}
            strview(const char* str, uint32_t len): data(str), len(len) {}

            template <uint32_t L> strview(const char (&str)[L]): data(str), len(L-1) {}

            operator const char*() const { return data; }
            char operator[](uint32_t i) const { return data[i]; } 
            uint32_t size() const { return len; }

            const char* begin() const { return data; }
            const char* end()   const { return data + len; }
            
            explicit operator bool() const { return data != nullptr; }
            bool operator !()        const { return data == nullptr; }

            bool operator>(const strview& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            bool operator<(const strview& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            bool operator==(const strview& b) const {
                return !(b.len != len || strncmp(data, b.data, len));
            }

            string allocate() const {
                string str{len};
                pk::copy(str.begin(), data, len);
                return str;
            }
    };
}