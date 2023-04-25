#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define GLCall(x)   \
    GLClearError(); \
    x;              \
    GLCheckError(#x, __FILE__, __LINE__);

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static void GLClearError() {
    // clang-format off
    while (glGetError() != GL_NO_ERROR);
    // clang-format on
}

static bool GLCheckError(const char *function, const char *file, int line) {
    GLenum error = GL_NO_ERROR;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "[OpenGL error] (" << error << ") " 
                  << function << " "
                  << file << ":" << line << std::endl;
        return true;
    }
    return false;
}

static ShaderProgramSource ParseShader(const std::string &filepath) {
    std::ifstream istream(filepath);

    enum ShaderType {
        None = -1,
        Vertex = 0,
        Fragment = 1,
    };

    ShaderType shaderType = ShaderType::None;
    std::string line;
    std::stringstream ss[2];
    while (getline(istream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                shaderType = ShaderType::Vertex;
            } else if (line.find("fragment") != std::string::npos) {
                shaderType = ShaderType::Fragment;
            }
        } else {
            ss[shaderType] << line << "\n";
        }
    }

    return {ss[ShaderType::Vertex].str(), ss[ShaderType::Fragment].str()};
}

static unsigned int compileShader(unsigned int type,
                                  const std::string &source) {
    GLCall(unsigned int id = glCreateShader(type));
    const char *src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, NULL));
    GLCall(glCompileShader(id));

    int result = 0;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length = 0;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char *message = (char *)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader) {
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "OpenGL Tutorial", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit() failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // https://stackoverflow.com/questions/62990972/why-is-opengl-giving-me-the-error-error-01-version-330-is-not-support
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glBindVertexArray(VAO));

    // clang-format off
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };
    unsigned int indices[] {
        0, 1, 2,
        2, 3, 0,
    };
    // clang-format on

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions,
                 GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices,
                 GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("./res/shader/basic.shader");
    // std::cout << "Vertex" << std::endl;
    // std::cout << source.vertexSource << std::endl;
    // std::cout << "Fragment" << std::endl;
    // std::cout << source.fragmentSource << std::endl;

    unsigned int shader =
        CreateShader(source.vertexSource, source.fragmentSource);

    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    assert(location != -1);

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

        if (r >= 1.0) {
            increment = -0.05f;
        } else if (r <= 0.0) {
            increment = 0.05f;
        }
        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteVertexArrays(1, &VAO));

    GLCall(glDeleteProgram(shader));
    glfwTerminate();
    return 0;
}
