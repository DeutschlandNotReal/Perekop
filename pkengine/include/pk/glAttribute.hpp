#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <pkutil/Array.hpp>

class glAttribute {
    int index{0};
    template <int d> void fmember(int stride, long long& offset, int L) {
        glVertexAttribPointer(index, L, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index++, d);
        offset += L * 4;
    }
    public:
        glAttribute(GLuint array) { glBindVertexArray(array); }
        glAttribute(GLuint* array) {
            glGenVertexArrays(1, array);
            glBindVertexArray(*array);
        }

        template <uint d, typename... T>glAttribute& object(T... member) {
            long long offset = 0;
            (fmember<d>((member+...)*4, offset, member), ...);
            return *this;
        }

        template <GLenum type> glAttribute& bind(GLuint b) { 
            glBindBuffer(type, b); 
            return *this; 
        }

        template <int L> glAttribute& make(GLuint* b) { glGenBuffers(L, b); return *this;}
        template <GLenum type, GLenum mode, typename T> glAttribute& data(GLuint buffer, const pk::Array<T>& content) {
            glBindBuffer(type, buffer);
            glBufferData(type, content.size() * sizeof(T), content.begin(), mode);
            return *this;
        }

        template <GLenum type> glAttribute& draw(int n_ind, int n_users) {
            glDrawElementsInstanced(GL_TRIANGLES, n_ind, type, 0, n_users);
            return *this;
        }
};