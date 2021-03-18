#include "Renderer.h"
#include "GL/glew.h"
#include <iostream>

const char *Renderer::getVertexShader() const{
    return  "#version 330 core\n"
            "layout(location = 0) in vec2 position;\n"
            "void main(){\n"
            "   gl_PointSize = 5.0;\n"
            "   gl_Position = vec4(position, 0, 1);\n"
            "};";
}
const char *Renderer::getFragmentShader() const{
    return  "#version 330 core\n"
            "layout(location = 0) out vec4 color;\n"
            "uniform vec4 u_Color;\n"
            "void main(){\n"
            "   color = u_Color;\n"
            "};";
}

Renderer::Renderer() : shader(getVertexShader(), getFragmentShader()) {}

void Renderer::addShape(const Shape* shape){
    shapes.push_back(shape);
}

void Renderer::replaceShape(unsigned int position, const Shape *shape){
    if (position < shapes.size()){
        delete shapes[position];
        shapes[position] = shape;
    } else{
        shapes.push_back(shape);
    }
}

void Renderer::drawShapes() const{
    shader.bind();
    for (std::vector<const Shape*>::const_iterator it = shapes.begin(); it != shapes.end(); it++){
        const Shape* shape = *it;
        shape->draw();
    }
}

void Renderer::clear() const{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::setPolygonColorFloat(float r, float g, float b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r, g, b, 1.0f);
}

void Renderer::setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r / 255.0, g / 255.0, b / 255.0, 1.0f);
}

void Renderer::initGLEW(){
    if (glewInit() != GLEW_OK){
        std::cout << "ERRORE di caricamento di Glew\n";
    }
    std::cout << "VERSION: " << glGetString(GL_VERSION) << "\n";

    // Returns the vendor
    auto vendor = glGetString(GL_VENDOR);
    // Returns a hint to the model
    auto model = glGetString(GL_RENDERER);

    std::cout << "VENDOR: " << vendor << "\n";
    std::cout << "MODEL: " << model << std::endl;

    glEnable(GL_PROGRAM_POINT_SIZE);
}
