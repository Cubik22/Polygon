#ifndef SHAPE_H
#define SHAPE_H

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Vector2f.h"
#include <vector>


class Shape{

public:
    virtual ~Shape();
    virtual void draw() const = 0;

    static const unsigned int DIMENSIONS;

protected:
    IndexBuffer ib;
    VertexBuffer vb;
    VertexArray va;

    // load vertices whatever dimension
    void loadVertices(const std::vector<float>& vertices);
    // load vertices for 2D with Vector2f
    void loadVertices2D(const std::vector<Vector2f>& vertices);
    // load indices with vector unsigned int
    void loadIndices(const std::vector<unsigned int>& indices);
    // load indices for lines of a polygon by simply passing indices order, last index is joined with the first
    void loadIndicesLinesPointIndices(const std::vector<unsigned int>& indices);
    void bindAll() const;
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
    ~LinesPointIndices() override;
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
