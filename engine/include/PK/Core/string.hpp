#pragma once
#include <PK/Core/memory.hpp>

namespace pk {
    class String {
        char* data{nullptr}; u32 len{0};

        public:
            char& operator[](u32 i) { return data[i]; }
            char  operator[](u32 i) const { return data[i]; }

            char* begin() { return data; }
            char* end()   { return data + len; }
            const char* begin() const { return data; }
            const char* end()   const { return data + len; }
            u32 size() const { return len; }

            String() = default;
            String(u32 L): data(pk::alloc(L + 1)), len(L) { data[L] = '\0'; }

            template <u32 L> String(const char (&str)[L]): data(pk::alloc(L)), len(L - 1) { 
                pk::copy(data, str, L);
            }

            String(const char* str) {
                len = strlen(str);
                data = pk::alloc(len+1);
                pk::copy(data, str, len+1);
            }

            String(String &&str) {
                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
            }

            String& operator=(String &&str) {
                if (&str == this) return *this;
                if (data) pk::free(data);

                data = str.data; len = str.len;
                str.data = nullptr; str.len = 0;
                return *this;
            }

            String(const String &str) {
                data = pk::alloc(str.len+1);
                pk::copy(data, str.data, str.len + 1);
                len = str.len;
            }

            String& operator=(const String& str) {
                if (&str == this) return *this;

                if (!data || len != str.len) {
                    if (data) pk::free(data);
                    data = pk::alloc(str.len + 1);
                }

                pk::copy(data, str.data, str.len);
                len = str.len;
                return *this;
            }

            operator char*() { return data; }
            operator const char*() const { return data; }

            explicit operator bool() const { return data != nullptr; }
            bool operator !()        const { return data == nullptr; }

            bool operator>(const String& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            bool operator<(const String& b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            bool operator==(const String& b) const {
                return !(b.len != len || std::strncmp(data, b.data, len));
            }

            ~String() { if (data) pk::free(data); }  
    };

    // string view
    class SView {
        const char* data{nullptr}; u32 len{0};

        public:
            template <u32 L> constexpr SView(const char (&str)[L]): data(str), len(L-1) {}
            SView() = default;
            SView(const String &str): data(str.begin()), len(str.size()) {}
            SView(const char* str): data(str), len(strlen(str)) {}

            constexpr SView(const char* str, const char* end): data(str), len(end - data) {}
            constexpr SView(const char* str, u32 len): data(str), len(len) {}

            operator const char*() const { return data; }
            char operator[](u32 i) const { return data[i]; } 

            const char* begin() const { return data; }
            const char* end()   const { return data + len; }
            u32 size()          const { return len; }

            explicit operator bool() const { return data != nullptr; }
            bool operator !()        const { return data == nullptr; }

            bool operator>(SView b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) > 0;
            }

            bool operator<(SView b) const {
                int n = b.len > len ? len : b.len;
                return std::strncmp(data, b.data, n) < 0;
            }
            
            bool operator==(SView b) const {
                return !(b.len != len || strncmp(data, b.data, len));
            }

            const char* find(char c) const {
                return (const char*)std::memchr(data, c, len);
            }

            // copies content as string (heap alloc!!)
            explicit operator String() const {
                String res{len};
                pk::copy(res.begin(), data, len);
                return res;
            }
    };
}