#include "pch.hpp"
#include <iostream>
#include <vector>

// g++ pkengine/src/main.cpp pkengine/lib/src/glad.c -Ipkengine/lib/include -Ipkengine/lib/include/glm -Lpkengine/lib/lib -lglfw3 -lopengl32 -o build/win.exe

using namespace glm;

// Vertex positions
std::vector<vec3> vertices = {
    // Base
    vec3(-0.5f, 0.0f, -0.5f),
    vec3( 0.5f, 0.0f, -0.5f),
    vec3( 0.5f, 0.0f,  0.5f),
    vec3(-0.5f, 0.0f,  0.5f),
    // Apex
    vec3(0.0f, 0.8f, 0.0f)
};

// Indices (EBO)
std::vector<unsigned int> indices = {
    // Base (two triangles)
    0,1,2,
    2,3,0,
    // Sides
    0,1,4,
    1,2,4,
    2,3,4,
    3,0,4
};

// Shader sources
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"void main(){ gl_Position = vec4(aPos,1.0); }\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n" 
"void main(){ FragColor = vec4(1.0,0.6,0.2,1.0); }\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"Pyramid EBO",NULL,NULL);
    if(!window) return -1;

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glViewport(0,0,800,600);
    glEnable(GL_DEPTH_TEST);

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create buffers
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(vec3),(void*)0);
    glEnableVertexAttribArray(0);

    // Main loop
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.1f,0.1f,0.15f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}