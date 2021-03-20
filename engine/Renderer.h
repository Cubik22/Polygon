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
    void drawShapes() const;
    void clear() const;
    void setPolygonColorFloat(float r, float g, float b) const;
    void setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const;

    static void initGLEW();
    static void setLineWidth(unsigned int width);

private:
    Shader shader;
    std::vector<const Shape*> shapes;
    std::vector<unsigned int> colors;

    const char* getVertexShader() const;
    const char* getFragmentShader() const;
};

#endif // RENDERER_H
