#include "Shader.h"
#include "GL/glew.h"
#include <stdio.h>
#include <alloca.h>

unsigned int Shader::create(const char *vertexShader, const char *fragmentShader) const{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fragmentShader);

    //link to one program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::compile(unsigned int type, const char *source) const{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    //error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    const char *nomeShader;
    if (type == GL_VERTEX_SHADER){
        nomeShader = "Vertex Shader";
    } else if (type == GL_FRAGMENT_SHADER){
        nomeShader = "Fragment Shader";
    }
    if (result == GL_FALSE){
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char *message = (char *)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        printf("ERROR: problemi nel compilamento di %s\n", nomeShader);
        printf("%s\n", message);
        glDeleteShader(id);
        return 0;
    } else {
        printf("%s compilato correttamente\n", nomeShader);
    }
    return id;
}

Shader::Shader(const char* vertexShader, const char* fragmentShader){
    rendererID = create(vertexShader, fragmentShader);
}

Shader::~Shader(){
    glDeleteProgram(rendererID);
}

void Shader::bind() const{
    glUseProgram(rendererID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

int Shader::getUniformLocation(const char *name) const{
    int location = glGetUniformLocation(rendererID, name);
    if (location == -1){
        printf("ATTENTION: uniform %s doesn't exist\n", name);
    }
return location;
}

void Shader::setUniformVec3(const char *name, float v[3]) const{
    glUniform3f(getUniformLocation(name), v[0], v[1], v[2]);
}

void Shader::setUniformVec4(const char *name, float v[4]) const{
    glUniform4f(getUniformLocation(name), v[0], v[1], v[2], v[3]);
}

void Shader::setUniform4f(const char *name, float a, float b, float c, float d) const{
    glUniform4f(getUniformLocation(name), a, b, c, d);
}
