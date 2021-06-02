#ifndef MESH_H
#define MESH_H

#include "Element.h"
#include "Segment.h"
#include <vector>


struct IndicesElement{
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> indicesInside;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> indicesOutside;
};

class Mesh{

public:
    Mesh(const Element& _element, const std::vector<Vector2f> _verticesBorder,
         unsigned int _numberX, unsigned int _numberY, float _elementWidth, float _elementHeight);

    Mesh(const Mesh&)                 = delete;
    Mesh(Mesh&&) noexcept             = delete;
    Mesh& operator=(const Mesh&)      = delete;
    Mesh& operator=(Mesh&&) noexcept  = delete;

    unsigned int getNumberPolygons() const;
    unsigned int getNumberElements() const;

    float getWidth() const;
    float getHeight() const;

    float getXMin() const;
    float getYMin() const;

    unsigned int getNumberX() const;
    unsigned int getNumberY() const;

    const std::vector<Vector2f>& getVertices(unsigned int x, unsigned int y) const;
    const std::vector<Vector2f>& getVertices(unsigned int i) const;

    const std::vector<std::shared_ptr<std::vector<Vector2f>>> getAllVertices() const;

    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& getIndices() const;

    std::vector<IndicesElement> cut();

    static void setDebugMode(bool mode);

    static void setXDebug(unsigned int _xDebug);
    static void setYDebug(unsigned int _yDebug);

private:
    std::vector<std::shared_ptr<std::vector<Vector2f>>> vertices;
    std::vector<std::shared_ptr<std::vector<unsigned int>>> indices;

    std::vector<Vector2f> verticesBorder;

    unsigned int numberX;
    unsigned int numberY;

    float elementWidth;
    float elementHeight;

    float width;
    float height;

    float xMin;
    float yMin;

    const unsigned int numberElements;
    const unsigned int numberPolygons;

    IndicesElement cutElement(std::vector<Vector2f>& verticesElement,
                              const std::vector<std::shared_ptr<std::vector<unsigned int>>>& startIndices);

    std::vector<Vector2f>& getVerticesPrivate(unsigned int x, unsigned int y) const;
    std::vector<Vector2f>& getVerticesPrivate(unsigned int i) const;

    static bool debug;

    static unsigned int xDebug;
    static unsigned int yDebug;
};

#endif // MESH_H
