#pragma once
#include <PK/Core/memory.hpp>

namespace pk {
    class string {
        char* data{nullptr}; u32 len{0};

        public:
            constexpr char& operator[](u32 i) { return data[i]; }
            constexpr char  operator[](u32 i) const { return data[i]; }

            constexpr char* begin() { return data; }
            constexpr char* end()   { return data + len; }
            constexpr const char* begin() const { return data; }
            constexpr const char* end()   const { return data + len; }
            constexpr u32 size() const { return len; }

            constexpr string() = default;
            constexpr string(u32 L): data(pk::alloc(L + 1)), len(L) { data[L] = '\0'; }

            template <u32 L> constexpr string(const char (&str)[L]): data(pk::alloc(L)), len(L - 1) { 
                pk::copy(data, str, L);
            }

            constexpr string(const char* str) {
                len = strlen(str);
                data = pk::alloc(len+1);
                pk::copy(data, str, len+1);
            }

            constexpr string(string &&str) {
                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
            }

            constexpr string& operator=(string &&str) {
                if (&str == this) return *this;
                if (data) pk::free(data);

                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
                return *this;
            }

            constexpr string(const string &str) {
                data = pk::alloc(str.len+1);
                pk::copy(data, str.data, str.len + 1);
                len = str.len;
            }

            constexpr string& operator=(const string& str) {
                if (&str == this) return *this;

                if (!data || len != str.len) {
                    if (data) pk::free(data);
                    data = pk::alloc(str.len + 1);
                }

                pk::copy(data, str.data, str.len + 1);
                len = str.len;
                return *this;
            }

            constexpr operator char*() { return data; }
            constexpr operator const char*() const { return data; }

            constexpr explicit operator bool() const { return data != nullptr; }
            constexpr bool operator !()        const { return data == nullptr; }

            constexpr bool operator>(const string& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            constexpr bool operator<(const string& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            constexpr bool operator==(const string& b) const {
                return !(b.len != len || std::strncmp(data, b.data, len));
            }

            constexpr ~string() { if (data) pk::free(data); }  
    };

    // string view
    class strview {
        const char* data{nullptr}; u32 len{0};

        public:
            template <u32 L> constexpr strview(const char (&str)[L]): data(str), len(L-1) {}
            constexpr strview() = default;
            constexpr strview(const string &str): data(str.begin()), len(str.size()) {}
            constexpr strview(const char* str): data(str), len(strlen(str)) {}

            constexpr strview(const char* str, const char* end): data(str), len(end - data) {}
            constexpr strview(const char* str, u32 len): data(str), len(len) {}

            constexpr operator const char*() const { return data; }
            constexpr char operator[](u32 i) const { return data[i]; } 

            constexpr const char* begin() const { return data; }
            constexpr const char* end()   const { return data + len; }
            constexpr u32 size()          const { return len; }

            constexpr explicit operator bool() const { return data != nullptr; }
            constexpr bool operator !()        const { return data == nullptr; }

            constexpr bool operator>(strview b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            constexpr bool operator<(strview b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            constexpr bool operator==(strview b) const {
                return !(b.len != len || strncmp(data, b.data, len));
            }

            constexpr const char* find(char c) const {
                return (const char*)std::memchr(data, c, len);
            }

            // copies content as string (heap alloc!!)
            constexpr explicit operator string() const {
                string res{len};
                pk::copy(res.begin(), data, len);
                return res;
            }
    };
}