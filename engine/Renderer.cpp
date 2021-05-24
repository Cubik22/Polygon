#include "Renderer.h"
#include "GL/glew.h"
#include "Logger.h"
#include <iostream>

Renderer::Renderer() : shader(getVertexShader(), getFragmentShader()) {}

void Renderer::addShape(const Shape* shape){
    shapes.push_back(shape);
}

void Renderer::replaceShape(unsigned int position, const Shape* shape){
    if (position < shapes.size()){
        if (shapes[position]){
            delete shapes[position];
        } else{
            LOG(LogLevel::WARN) << "shape " << position << " was nullptr";
        }
        shapes[position] = shape;
    } else{
        addShape(shape);
    }
}

void Renderer::removeLastShape(){
    if (shapes.size() > 0){
        if (shapes[shapes.size() - 1]){
            delete shapes[shapes.size() - 1];
        } else{
            LOG(LogLevel::WARN) << "last shape " << shapes.size() - 1 << " was nullptr";
        }
        shapes.pop_back();
    }
}

void Renderer::removeAllShapes(){
    while (shapes.size() > 0){
        removeLastShape();
    }
}

void Renderer::drawShapes() const{
    shader.bind();
    for (unsigned int i = 0; i < shapes.size(); i++){
        const Shape* shape = shapes[i];
        if (!shape || shape == nullptr){
            LOG(LogLevel::DEBUG) << "shape " << i << " is nullptr";
            continue;
        }
        setPolygonColorFloat(shape->getRGB());
        shape->draw();
    }
}

void Renderer::clear() const{
    glClear(GL_COLOR_BUFFER_BIT);
}

unsigned int Renderer::getNumberShapes() const{
    return shapes.size();
}

bool Renderer::PRINT_INFO = true;

void Renderer::initGLEW(){
    if (glewInit() != GLEW_OK){
        //LoggerStatic::Error("problems while initializing Glew");
        LOG(LogLevel::ERROR) << "problems while initializing Glew";
    } else{
        //LoggerStatic::Message("Glew initialized correctly");
        LOG(LogLevel::INFO) << "Glew initialized correctly";
    }

    if (Renderer::PRINT_INFO){
        //LoggerStatic::Message((const std::string&)"VERSION: " + (const char*)glGetString(GL_VERSION));
        LOG(LogLevel::INFO) << "VERSION: " << glGetString(GL_VERSION);

        // Returns the vendor
        auto vendor = (const char*)glGetString(GL_VENDOR);
        // Returns a hint to the model
        auto model = (const char*)glGetString(GL_RENDERER);

        //LoggerStatic::Message((const std::string&)"VENDOR: " + vendor);
        LOG(LogLevel::INFO) << "VENDOR: " << vendor;
        //LoggerStatic::Message((const std::string&)"MODEL: " + model);
        LOG(LogLevel::INFO) << "MODEL: " << model;
    }

    // enable setting gl_PointSize in Vertex Shader
    glEnable(GL_PROGRAM_POINT_SIZE);

    //set the default line width to 3
    glLineWidth(3);
}

void Renderer::setLineWidth(unsigned int width){
    glLineWidth(width);
}

unsigned int Renderer::NumberColor = 0;

std::vector<float> Renderer::getNextColor(){
    Renderer::NumberColor++;
    return getLastColor();
}

std::vector<float> Renderer::getLastColor(){
    unsigned int colorsSize = Colors.size();
    return {Colors[(3 * Renderer::NumberColor)     % colorsSize] / 255.0f,
            Colors[(3 * Renderer::NumberColor + 1) % colorsSize] / 255.0f,
            Colors[(3 * Renderer::NumberColor + 2) % colorsSize] / 255.0f};
}

std::vector<float> Renderer::getColor(RendColor color){
    unsigned int colorsSize = Colors.size();
    return {Colors[(3 * (unsigned int)color)     % colorsSize] / 255.0f,
            Colors[(3 * (unsigned int)color + 1) % colorsSize] / 255.0f,
            Colors[(3 * (unsigned int)color + 2) % colorsSize] / 255.0f};
}

// PRIVATE

void Renderer::setPolygonColorFloat(float r, float g, float b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r, g, b, 1.0f);
}

void Renderer::setPolygonColorFloat(std::vector<float> rgb) const{
    if (rgb.size() != 3){
        LOG(LogLevel::ERROR) << "float vector color should have size 3";
        return;
    }
    shader.bind();
    shader.setUniform4f("u_Color", rgb[0], rgb[1], rgb[2], 1.0f);
}

void Renderer::setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r / 255.0, g / 255.0, b / 255.0, 1.0f);
}

void Renderer::setPolygonColorRGB(std::vector<unsigned int> rgb) const{
    if (rgb.size() != 3){
        LOG(LogLevel::ERROR) << "unsigned int vector color should have size 3";
        return;
    }
    shader.bind();
    shader.setUniform4f("u_Color", rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0, 1.0f);
}

std::vector<unsigned int> Renderer::Colors = {
    255,   0,   0,      //red
      0, 255,   0,      //lime
      0,   0, 255,      //blue
    255, 255,   0,      //yellow
      0, 255, 255,      //cyan
    255, 250, 205,      //lemon
    255,   0, 255,      //magenta
    139,   0,   0,      //dark red
      0, 128,   0,      //green
    138,  43, 226,      //blue violet
    255,  20, 147       //deep pink
};

const char *Renderer::getVertexShader(){
    static const char* vertexShader =
            "#version 330 core\n"
            "layout(location = 0) in vec2 position;\n"
            "void main(){\n"
            "   gl_PointSize = 5.0;\n"
            "   gl_Position = vec4(position, 0, 1);\n"
            "};";
    return vertexShader;
}

const char *Renderer::getFragmentShader(){
    static const char* fragmentShader =
            "#version 330 core\n"
            "layout(location = 0) out vec4 color;\n"
            "uniform vec4 u_Color;\n"
            "void main(){\n"
            "   color = u_Color;\n"
            "};";
    return fragmentShader;
}
