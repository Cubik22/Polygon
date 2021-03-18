#include "Shape.h"
#include "GL/glew.h"

const unsigned int Shape::DIMENSIONS = 2;

Shape::~Shape() {}

void Shape::loadVertices(const std::vector<float> &vertices){
    unsigned int numberVertices = vertices.size();
    float verticesArray[numberVertices];
    std::copy(vertices.begin(), vertices.end(), verticesArray);
    vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float), true);

    VertexBufferLayout layout;
    layout.pushElement(GL_FLOAT, Shape::DIMENSIONS);
    va.addBuffer(vb, layout);
}

void Shape::loadVertices2D(const std::vector<Vector2f> &vertices){
    unsigned int numberVertices = 2 * vertices.size();
    float verticesArray[numberVertices];
    for (unsigned int i = 0; i < vertices.size(); i++){
        verticesArray[2 * i] = vertices[i].x;
        verticesArray[2 * i + 1] = vertices[i].y;
    }
    vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float), true);

    VertexBufferLayout layout;
    layout.pushElement(GL_FLOAT, Shape::DIMENSIONS);
    va.addBuffer(vb, layout);
}

void Shape::loadIndices(const std::vector<unsigned int> &indices){
    unsigned int numberIndices = indices.size();
    unsigned int indicesArray[numberIndices];
    std::copy(indices.begin(), indices.end(), indicesArray);
    ib.sendData(indicesArray, numberIndices, true);
}

void Shape::bindAll() const{
    vb.bind();
    va.bind();
    ib.bind();
}

Points::Points(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndices(indices);
}

Points::Points(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndices(indices);
}

Points::~Points() {}

void Points::draw() const{
    bindAll();
    glDrawElements(GL_POINTS, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}

Lines::Lines(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndices(indices);
}

Lines::Lines(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndices(indices);
}

Lines::~Lines() {}

void Lines::draw() const{
    bindAll();
    glDrawElements(GL_LINES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}

Triangles::Triangles(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndices(indices);
}

Triangles::Triangles(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndices(indices);
}

Triangles::~Triangles() {}

void Triangles::draw() const{
    bindAll();
    glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}
