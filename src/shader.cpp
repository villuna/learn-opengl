#include "shader.h"
#include <optional>
#include <stdexcept>
#include <sstream>

// Checks to see if the shader compiled correctly. Returns a string containing an error message if
// it encountered an error.
std::optional<std::string> checkShader(unsigned int shaderId, const char *stage) {
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::stringstream errMsg;
        errMsg << "Error compiling " << stage << " shader: ";
        errMsg << infoLog;
        return errMsg.str();
    }

    return std::nullopt;
}

std::optional<std::string> checkShaderProgram(unsigned int programId) {
    int success;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::stringstream errMsg;
        errMsg << "Error linking shader program: ";
        errMsg << infoLog;
        return errMsg.str();
    }

    return std::nullopt;
}

Shader::Shader(const char *vertex, const char *fragment) {
    // Compile shaders
    unsigned int vertexShader, fragmentShader;
    std::optional<std::string> errMsg;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);
    if ((errMsg = checkShader(vertexShader, "vertex")).has_value()) {
        glDeleteShader(vertexShader);
        throw std::runtime_error(*errMsg);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    if ((errMsg = checkShader(fragmentShader, "fragment")).has_value()) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error(*errMsg);
    }

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    if ((errMsg = checkShaderProgram(id)).has_value()) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(id);
        throw std::runtime_error(*errMsg);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (id != GL_NONE)
        glDeleteProgram(id);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
