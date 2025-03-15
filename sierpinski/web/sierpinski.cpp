#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#include <iostream>
#include <vector>

// Struct to encapsulate state variables
struct AppState {
    std::vector<float> vertices;
    GLuint vbo, shaderProgram;
    GLFWwindow* window;
};

const char* vertexShaderSource = R"(
attribute vec2 position;
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
})";

const char* fragmentShaderSource = R"(
precision mediump float;
void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); // white
})";

void generateSierpinski(std::vector<float>& vertices, float p0[2], float p1[2], float p2[2], int depth) {
    if (depth == 0) {
        vertices.push_back(p0[0]); vertices.push_back(p0[1]);
        vertices.push_back(p1[0]); vertices.push_back(p1[1]);
        vertices.push_back(p2[0]); vertices.push_back(p2[1]);
    } else {
        float m0[2] = { (p0[0] + p1[0]) / 2, (p0[1] + p1[1]) / 2 };
        float m1[2] = { (p1[0] + p2[0]) / 2, (p1[1] + p2[1]) / 2 };
        float m2[2] = { (p0[0] + p2[0]) / 2, (p0[1] + p2[1]) / 2 };

        generateSierpinski(vertices, m2, m1, p2, depth - 1);
        generateSierpinski(vertices, p0, m0, m2, depth - 1);
        generateSierpinski(vertices, m0, p1, m1, depth - 1);
    }
}

void mainloop(void* arg) {
    AppState* state = static_cast<AppState*>(arg);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(state->shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);

    // Enable and set vertex attributes
    GLint posAttrib = glGetAttribLocation(state->shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, state->vertices.size() / 2);

    glfwSwapBuffers(state->window);
    glfwPollEvents();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Sierpiński Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    AppState state;
    state.window = window;

    float p0[2] = { -0.5f, -0.5f };
    float p1[2] = { 0.5f, -0.5f };
    float p2[2] = { 0.0f, 0.5f };
    int depth = 2;
    generateSierpinski(state.vertices, p0, p1, p2, depth);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Error:\n" << infoLog << std::endl;
        return -1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Error:\n" << infoLog << std::endl;
        return -1;
    }

    state.shaderProgram = glCreateProgram();
    glAttachShader(state.shaderProgram, vertexShader);
    glAttachShader(state.shaderProgram, fragmentShader);
    glLinkProgram(state.shaderProgram);

    glGetProgramiv(state.shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(state.shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Error:\n" << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenBuffers(1, &state.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
    glBufferData(GL_ARRAY_BUFFER, state.vertices.size() * sizeof(float), state.vertices.data(), GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    emscripten_set_main_loop_arg(mainloop, &state, 0, 1);

    glDeleteProgram(state.shaderProgram);
    glfwTerminate();
    return 0;
}
