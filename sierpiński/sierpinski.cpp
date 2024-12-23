#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 position;
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
})";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0, 1.0, 1.0, 1.0); //white
})";

void generateSierpinski(std::vector<float>& vertices, float p0[2], float p1[2], float p2[2], int depth)
{
    if (depth == 0)
    {
        // Base Case
        vertices.push_back(p0[0]); // x of first point
        vertices.push_back(p0[1]); // y of first point
        vertices.push_back(p1[0]); // x of second point
        vertices.push_back(p1[1]); // y of second point
        vertices.push_back(p2[0]); // x of third point
        vertices.push_back(p2[1]); // y of third point
    }
    else
    {
        // mid - point
        float m0[2] = { (p0[0] + p1[0])/2, (p0[1]+p1[1])/2 };
        float m1[2] = { (p1[0] + p2[0])/2, (p1[1]+p2[1])/2 }; 
        float m2[2] = { (p0[0] + p2[0])/2, (p0[1]+p2[1])/2 }; 

        // Recurse on the 3 new edges
        generateSierpinski(vertices, m2, m1, p2, depth - 1); // depth is the recursive variable
        generateSierpinski(vertices, p0, m0, m2, depth - 1);
        generateSierpinski(vertices, m0, p1, m1, depth - 1);
    }
}


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "sierpiński", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initializing GLEW" << std::endl;
        return -1;
    }

    std::vector<float> vertices;

    // Initial points of the equilateral triangle
    float p0[2] = {-0.5f, -0.5f};
    float p1[2] = {0.5f, -0.5f};
    float p2[2] = {0.0f, 0.5f};

    // Depth of recursion ( 11 or 12 gives kinda sax result)
    int depth = 12;

    generateSierpinski(vertices, p0, p1, p2, depth);
    generateSierpinski(vertices, p1, p2, p0, depth);
    generateSierpinski(vertices, p2, p0, p1, depth);

    // Create and compile the vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link the shaders into a program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create and bind a Vertex Array Object (VAO)
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object (VBO)
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Set up the vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, vertices.size() / 2);  // Draw the pattern as lines

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
