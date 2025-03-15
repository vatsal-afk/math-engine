#include <emscripten.h>
#include <GLES3/gl3.h>
#include <vector>
#include <iostream>

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

// Point structure for vertices
struct Point {
    float x, y;
};

std::vector<Point> vertices;
int depth = 3;

// Shader sources
const char* vertexShaderSource = R"(
    #version 300 es
    layout(location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 300 es
    precision mediump float;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.5, 0.8, 0.2, 1.0);
    }
)";

// Function to compile a shader
GLuint CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader Compilation Failed\n" << infoLog << std::endl;
    }
    return shader;
}

// Generate Sierpiński Triangle vertices
void GenerateSierpinski(int depth) {
    vertices.clear();
    Point p1 = {-0.5f, -0.5f};
    Point p2 = {0.5f, -0.5f};
    Point p3 = {0.0f, 0.5f};

    std::vector<Point> current = {p1, p2, p3};

    for (int i = 0; i < depth; ++i) {
        std::vector<Point> next;
        for (size_t j = 0; j < current.size(); j += 3) {
            Point mid1 = {(current[j].x + current[j + 1].x) / 2, (current[j].y + current[j + 1].y) / 2};
            Point mid2 = {(current[j + 1].x + current[j + 2].x) / 2, (current[j + 1].y + current[j + 2].y) / 2};
            Point mid3 = {(current[j].x + current[j + 2].x) / 2, (current[j].y + current[j + 2].y) / 2};

            next.push_back(current[j]);
            next.push_back(mid1);
            next.push_back(mid3);

            next.push_back(mid1);
            next.push_back(current[j + 1]);
            next.push_back(mid2);

            next.push_back(mid3);
            next.push_back(mid2);
            next.push_back(current[j + 2]);
        }
        current = next;
    }
    vertices = current;
}

// State variables
GLFWwindow* window;
GLuint VBO, VAO, shaderProgram;

// Rendering loop
void MainLoop() {
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui UI
    ImGui::Begin("Controls");
    if (ImGui::SliderInt("Depth", &depth, 0, 8)) {
        GenerateSierpinski(depth);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Point), vertices.data(), GL_DYNAMIC_DRAW);
    }
    ImGui::End();

    // Render OpenGL
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(800, 600, "Sierpiński Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // Compile shaders and create program
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate vertices
    GenerateSierpinski(depth);

    // Setup VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Point), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Run main loop
    emscripten_set_main_loop(MainLoop, 0, 1);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
