#ifndef SHAPE_H
#define SHAPE_H

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Vector2f.h"
#include <vector>


enum class VerticesType{
    Float,
    Vector2f
};

enum class IndicesType{
    UnsignedInt
};

enum class GeometricPrimitive{
    Point,
    Line,
    LinePointOpen,
    LinePointClosed,
    Triangle,
    Polygon
};

struct ShapeComposition{
    const void* vertices =          nullptr;
    const void* indices =           nullptr;
    unsigned int numberVertices =   0;
    unsigned int numberIndices =    0;
    VerticesType verticesType =     VerticesType::Float;
    IndicesType indicesType =       IndicesType::UnsignedInt;
    GeometricPrimitive primitive =  GeometricPrimitive::Point;
    float r =                       1.0;
    float g =                       1.0;
    float b =                       1.0;
};

class Shape{

public:
    Shape();
    Shape(const void* vertices, const void* indices, unsigned int numberVertices, unsigned int numberIndices,
          VerticesType verticesType, IndicesType indicesType, GeometricPrimitive primitive,
          float r = 1.0, float g = 1.0, float b = 1.0);
    Shape(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices,
          GeometricPrimitive primitive, float r = 1.0, float g = 1.0, float b = 1.0);
    Shape(const std::vector<Vector2f>& vertices, GeometricPrimitive primitive, float r = 1.0, float g = 1.0, float b = 1.0);
    Shape(ShapeComposition composition);

    virtual ~Shape();

    virtual void draw() const;

    GeometricPrimitive getPrimitive();

    float getR() const;
    float getG() const;
    float getB() const;

    std::vector<float> getRGB() const;

    static const unsigned int DIMENSIONS;

protected:
    IndexBuffer ib;
    VertexBuffer vb;
    VertexArray va;

    GeometricPrimitive primitive;

    // colors
    float r;
    float g;
    float b;

    // load vertices whatever dimension
    void loadVertices(const std::vector<float>& vertices);
    // load vertices for 2D with Vector2f
    void loadVertices2D(const std::vector<Vector2f>& vertices);
    // load indices with vector unsigned int
    void loadIndices(const std::vector<unsigned int>& indices);
    // load indices for lines of a polygon by simply passing indices order, last index is connected with the first
    void loadIndicesLinesPointIndices(const std::vector<unsigned int>& indices);
    // load indices for lines of a polygon by simply passing indices order
    void loadIndicesLinesPointIndicesOpen(const std::vector<unsigned int>& indices);
    // load default indices: 0, 1, 2, 3 ecc
    // open if you don't want to connect last vertex with first one
    void loadIndicesDefault(unsigned int size, bool open);

    void bindAll() const;

private:
    unsigned int getNumberGeometricPrimitive(GeometricPrimitive primitive);
    //unsigned int getNumberIndicesOrder(IndicesOrder order);
};

// list of points, indices specify wich to choose from vertices

class Points : public Shape{

public:
    Points(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Points(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Points() override;
    void draw() const override;
};

// list of lines, even indices specify starting vertex, odd indices specify end vertex

class Lines : public Shape{

public:
    Lines(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Lines(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Lines() override;
    void draw() const override;
};

// list of lines of a polygon, indices specify order of vertices, last vertex is joined with the first one

class LinesPointIndices : public Shape{

public:
    LinesPointIndices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    LinesPointIndices(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    // default indices: 0, 1, 2, 3 ecc
    LinesPointIndices(const std::vector<Vector2f>& vertices);
    ~LinesPointIndices() override;
    void draw() const override;
};

// list of lines of a polygon, indices specify order of vertices, last vertex is not joined with the first one

class LinesPointIndicesOpen : public Shape{

public:
    LinesPointIndicesOpen(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    LinesPointIndicesOpen(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    // default indices: 0, 1, 2, 3 ecc
    LinesPointIndicesOpen(const std::vector<Vector2f>& vertices);
    ~LinesPointIndicesOpen() override;
    void draw() const override;
};

class Triangles : public Shape{

public:
    Triangles(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Triangles(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Triangles() override;
    void draw() const override;
};

// polygon not concave (using GL_POLYGON), indices specify list of points

class Figure : public Shape{

public:
    Figure(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Figure(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Figure() override;
    void draw() const override;
};

#endif // SHAPE_H
