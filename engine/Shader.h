#ifndef SHADER_H
#define SHADER_H

#define BLACK 0.0, 0.0, 0,0
#define WHITE 1.0, 1.0, 1.0


class Shader{

public:
    Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    void bind() const;
    void unbind() const;
    int getUniformLocation(const char* name) const;
    void setUniformVec3(const char* name, float v[3]) const;
    void setUniformVec4(const char* name, float v[4]) const;
    void setUniform3f(const char* name, float a, float b, float c) const;
    void setUniform4f(const char* name, float a, float b, float c, float d) const;

private:
    unsigned int rendererID;
    unsigned int create(const char* vertexShader, const char* fragmentShader) const;
    unsigned int compile(unsigned int type, const char* source) const;
};

#endif // SHADER_H
