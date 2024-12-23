#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

// Vertex and Fragment shader source code as strings
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
    color = vec4(0.0, 0.8, 1.0, 1.0); // Light blue color for snowflake
})";

// Recursive function to generate Koch snowflake vertices
void generateKochSnowflake(std::vector<float>& vertices, float p0[2], float p1[2], int depth)
{
    if (depth == 0)
    {
        // Base case: add the line from p0 to p1
        vertices.push_back(p0[0]);
        vertices.push_back(p0[1]);
        vertices.push_back(p1[0]);
        vertices.push_back(p1[1]);
    }
    else
    {
        // Subdivide the line segment p0 -> p1 into 3 parts
        float dx = p1[0] - p0[0];
        float dy = p1[1] - p0[1];

        // Calculate the 3 intermediate points
        float p2[2] = { p0[0] + dx / 3.0f, p0[1] + dy / 3.0f };
        float p3[2] = { p0[0] + 2.0f * dx / 3.0f, p0[1] + 2.0f * dy / 3.0f };

        // Calculate the bump point (perpendicular triangle)
        float angle = M_PI / 3.0f; // 60 degrees
        float length = sqrt(dx * dx + dy * dy) / 3.0f;
        float bumpX = (p2[0] + p3[0]) / 2.0f + length * cos(angle + atan2(dy, dx));
        float bumpY = (p2[1] + p3[1]) / 2.0f + length * sin(angle + atan2(dy, dx));

        // Create bump array
        float bump[2] = { bumpX, bumpY };

        // Recurse on the 3 new edges
        generateKochSnowflake(vertices, p0, p2, depth - 1);
        generateKochSnowflake(vertices, p2, bump, depth - 1);
        generateKochSnowflake(vertices, bump, p3, depth - 1);
        generateKochSnowflake(vertices, p3, p1, depth - 1);
    }
}


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Koch Snowflake", NULL, NULL);
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

    // Vertices to hold Koch snowflake points
    std::vector<float> vertices;

    // Initial points of the equilateral triangle
    float p0[2] = {-0.5f, -0.5f};
    float p1[2] = {0.5f, -0.5f};
    float p2[2] = {0.0f, 0.5f};

    // Depth of recursion (try 3 or 4 for a clear snowflake)
    int depth = 4;

    // Generate Koch snowflake vertices
    generateKochSnowflake(vertices, p0, p1, depth);
    generateKochSnowflake(vertices, p1, p2, depth);
    generateKochSnowflake(vertices, p2, p0, depth);

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
        glDrawArrays(GL_LINES, 0, vertices.size() / 2);  // Draw the Koch snowflake as lines

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
