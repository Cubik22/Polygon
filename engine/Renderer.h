#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>


class Renderer{

public:
    Renderer();

    void addShape(const Shape* shape);
    void replaceShape(unsigned int position, const Shape* shape);
    void removeLastShape();
    void removeAllShapes();
    void drawShapes() const;
    void clear() const;

    static bool PRINT_INFO;

    static void initGLEW();
    static void setLineWidth(unsigned int width);

    static std::vector<float> getNextColor();
    static std::vector<float> getLastColor();

private:
    Shader shader;
    std::vector<const Shape*> shapes;

    void setPolygonColorFloat(float r, float g, float b) const;
    void setPolygonColorFloat(std::vector<float> rgb) const;
    void setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const;
    void setPolygonColorRGB(std::vector<unsigned int> rgb) const;

    static unsigned int NumberColor;
    static std::vector<unsigned int> Colors;

    static const char* getVertexShader();
    static const char* getFragmentShader();
};

#endif // RENDERER_H
