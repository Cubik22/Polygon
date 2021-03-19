#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>

class Renderer{
private:
    std::vector<const Shape*> shapes;
    std::vector<unsigned int> colors;

    Shader shader;
    const char* getVertexShader() const;
    const char* getFragmentShader() const;
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
};

#endif // RENDERER_H
