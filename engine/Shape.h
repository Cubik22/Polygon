#ifndef SHAPE_H
#define SHAPE_H

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Vector2f.h"
#include <vector>

class Shape{
protected:
    IndexBuffer ib;
    VertexBuffer vb;
    VertexArray va;

    void loadVertices(const std::vector<float>& vertices);
    void loadVertices2D(const std::vector<Vector2f>& vertices);
    void loadIndices(const std::vector<unsigned int>& indices);
    void bindAll() const;
public:
    virtual ~Shape();
    virtual void draw() const = 0;

    static const unsigned int DIMENSIONS;
};

class Points : public Shape{
public:
    Points(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Points(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Points();
    void draw() const override;
};

class Lines : public Shape{
public:
    Lines(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Lines(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Lines();
    void draw() const override;
};

class Triangles : public Shape{
public:
    Triangles(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    Triangles(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    ~Triangles();
    void draw() const override;
};

#endif // SHAPE_H
