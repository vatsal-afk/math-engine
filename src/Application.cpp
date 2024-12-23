# include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <csignal>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos)
        {
            if(line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if(line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = &source[0]; // source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error Handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // allocate on stack here instead of heap
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " 
        << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
        << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    } // alloca - allocate on the stack dynamically

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) // glewInit only after creating a valid openGL rendering context
    {
        std::cout << "Error initializing GLEW" << std::endl;
        return -1;
    }

    float positions[] = {
       -0.5f, -0.5f, // 0
        0.5f, 0.5f,  // 1
        0.5f, -0.5f, // 2
       -0.5f, 0.5f   // 3
    };

    unsigned int indices[] = { // tho int are 4 bytes, can use char or short
        2,1,0,
        0,3,1
    } ;

    unsigned int vao; // vertex attribute object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VertexArray va;
    VertexBuffer vb(positions, 4 * 2 *  sizeof(float));
    // va.AddBuffer(vb);
    // BufferLayout layout;
    // layout.Push<float>(3);
    // va.AddLayout(layout);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    IndexBuffer ib(indices, 6);

    ShaderProgramSource source = ParseShader("./res/shaders/basic.shader"); //  filepath should be relative to the executable!!!
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    //glUseProgram(shader); // bound shader

    int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);
    glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

    // ------
    glBindVertexArray(0);
    glUseProgram(0); // bound shader
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) // render loop (like game loop)
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // ------
        glUseProgram(shader); // bound shader
        glUniform4f(location, r, 0.3f, 0.8f, 1.0f);
        
        glBindVertexArray(vao);
        // va.Bind();
        ib.Bind();
        // -------

        // error handling using the defined macros and functions
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // null as already bound

        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}