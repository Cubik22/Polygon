#include "Shape.h"
#include "GL/glew.h"
#include <stdlib.h>

#include "Logger.h"

const unsigned int Shape::DIMENSIONS = 2;

Shape::Shape() {}

Shape::Shape(const void* vertices, const void* indices, unsigned int numberVertices, unsigned int numberIndices,
             VerticesType verticesType, IndicesType indicesType, GeometricPrimitive _primitive,
             float _r, float _g, float _b) : primitive{_primitive}, r{_r}, g{_g}, b{_b}{
    if (!vertices || vertices == nullptr){
        LOG(LogLevel::ERROR) << "Created shape with null vertices";
        return;
    }
    if (numberVertices == 0){
        LOG(LogLevel::ERROR) << "Created shape with 0 vertices";
        return;
    } else if (numberVertices == 1 && primitive != GeometricPrimitive::Point){
        //LOG(LogLevel::WARN) << "Created non point shape with 1 vertex";
        return;
    }
    unsigned int numberFloat = Shape::DIMENSIONS * numberVertices;
    float verticesArray[numberFloat];
    if (verticesType == VerticesType::Float){
        const std::vector<float>& verticesFloat = (const std::vector<float>&) *((const std::vector<float>*) vertices);
        if (verticesFloat.size() == 0){
            LOG(LogLevel::ERROR) << "Number of vertices can not be 0 (Float)";
            return;
        } else if (numberFloat > verticesFloat.size()){
            LOG(LogLevel::ERROR) << "Number of vertices is bigger than vertices size (Float)";
            return;
        }
        std::copy(verticesFloat.begin(), verticesFloat.end(), verticesArray);

        vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float));
        VertexBufferLayout layout;
        layout.pushElement(GL_FLOAT, Shape::DIMENSIONS);
        va.addBuffer(vb, layout);
    } else if (verticesType == VerticesType::Vector2f){
        const std::vector<Vector2f>& verticesVector = (const std::vector<Vector2f>&) *((const std::vector<Vector2f>*) vertices);
        if (verticesVector.size() == 0){
            LOG(LogLevel::ERROR) << "Number of vertices can not be 0 (Vector2f)";
            return;
        } else if (numberVertices > verticesVector.size()){
            LOG(LogLevel::ERROR) << "Number of vertices is bigger than vertices size (Vector2f)";
            return;
        }
        for (unsigned int v = 0; v < numberVertices; v++){
            verticesArray[Shape::DIMENSIONS * v] = verticesVector[v].x;
            verticesArray[Shape::DIMENSIONS * v + 1] = verticesVector[v].y;
        }

        vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float));
        VertexBufferLayout layout;
        layout.pushElement(GL_FLOAT, Shape::DIMENSIONS);
        va.addBuffer(vb, layout);
    }

    if (indicesType == IndicesType::UnsignedInt){
        if (!indices || indices == nullptr){
            if (primitive != GeometricPrimitive::Point && primitive != GeometricPrimitive::Polygon &&
                primitive != GeometricPrimitive::LinePointOpen && primitive != GeometricPrimitive::LinePointClosed){
                LOG(LogLevel::ERROR) << "indices pointer can not be null";
                return;
            }
            if (primitive == GeometricPrimitive::Point || primitive == GeometricPrimitive::Polygon){
                unsigned int indicesArray[numberVertices];
                for (unsigned int i = 0; i < numberVertices; i++){
                    indicesArray[i] = i;
                }
                ib.sendData(indicesArray, numberVertices);
            } else if (primitive == GeometricPrimitive::LinePointOpen){
                int sizeIndicesArray = (numberVertices - 1) * 2;
                if (sizeIndicesArray <= 0){
                    LOG(LogLevel::ERROR) << "size indices array is negative or zero";
                    return;
                }
                unsigned int indicesArray[(numberVertices - 1) * 2];
                for (unsigned int i = 0; i < numberVertices - 1; i++){
                    indicesArray[2 * i] = i;
                    indicesArray[2 * i + 1] = (i + 1) % numberVertices;
                }
                ib.sendData(indicesArray, numberVertices * 2);
            } else if (primitive == GeometricPrimitive::LinePointClosed){
                unsigned int indicesArray[numberVertices * 2];
                for (unsigned int i = 0; i < numberVertices; i++){
                    indicesArray[2 * i] = i;
                    indicesArray[2 * i + 1] = (i + 1) % numberVertices;
                }
                ib.sendData(indicesArray, numberVertices * 2);
            }
        } else{
            if (numberIndices == 0){
                LOG(LogLevel::ERROR) << "Created shape with 0 indices";
                return;
            }
            const std::vector<unsigned int>& indicesInt = (const std::vector<unsigned int>&) *((const std::vector<unsigned int>*) indices);
            if (numberIndices > indicesInt.size()){
                LOG(LogLevel::ERROR) << "Number of indices is bigger than indices size (Unsigned int)";
                return;
            }
            if (primitive == GeometricPrimitive::Point || primitive == GeometricPrimitive::Line ||
                primitive == GeometricPrimitive::Triangle || primitive == GeometricPrimitive::Polygon){
                unsigned int indicesArray[numberIndices];
                std::copy(indicesInt.begin(), indicesInt.end(), indicesArray);
                ib.sendData(indicesArray, numberIndices);
            } else if (primitive == GeometricPrimitive::LinePointOpen){
                unsigned int number = 2 * (numberIndices - 1);
                unsigned int indicesArray[number];
                for (unsigned int i = 0; i < numberIndices - 1; i++){
                    indicesArray[2 * i] = indicesInt[i];
                    indicesArray[2 * i + 1] = indicesInt[i + 1];
                }
                ib.sendData(indicesArray, number);
            } else if (primitive == GeometricPrimitive::LinePointClosed){
                unsigned int number = 2 * numberIndices;
                unsigned int indicesArray[number];
                for (unsigned int i = 0; i < numberIndices; i++){
                    indicesArray[2 * i] = indicesInt[i];
                    indicesArray[2 * i + 1] = indicesInt[(i + 1) % numberIndices];
                }
                ib.sendData(indicesArray, number);
            }
        }
    }
}

Shape::Shape(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices,
             GeometricPrimitive primitive, float r, float g, float b) :
    Shape(&vertices, &indices, vertices.size(), indices.size(), VerticesType::Vector2f, IndicesType::UnsignedInt, primitive, r, g, b) {}

Shape::Shape(const std::vector<Vector2f>& vertices, GeometricPrimitive primitive, float r, float g, float b) :
    Shape(&vertices, nullptr, vertices.size(), 0, VerticesType::Vector2f, IndicesType::UnsignedInt, primitive, r, g, b) {}

Shape::Shape(ShapeComposition comp) : Shape(comp.vertices, comp.indices, comp.numberVertices, comp.numberIndices,
                                            comp.verticesType, comp.indicesType, comp.primitive,
                                            comp.r, comp.g, comp.b) {}

Shape::~Shape() {}

void Shape::draw() const{
    bindAll();
    if (primitive == GeometricPrimitive::Point){
        glDrawElements(GL_POINTS, ib.getCount(), GL_UNSIGNED_INT, nullptr);
    } else if (primitive == GeometricPrimitive::Line ||
               primitive == GeometricPrimitive::LinePointOpen || primitive == GeometricPrimitive::LinePointClosed){
        glDrawElements(GL_LINES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
    } else if (primitive == GeometricPrimitive::Triangle){
        glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
    } else if (primitive == GeometricPrimitive::Polygon){
        glDrawElements(GL_POLYGON, ib.getCount(), GL_UNSIGNED_INT, nullptr);
    }
}

GeometricPrimitive Shape::getPrimitive(){
    return primitive;
}

float Shape::getR() const{
    return r;
}

float Shape::getG() const{
    return g;
}

float Shape::getB() const{
    return b;
}

std::vector<float> Shape::getRGB() const{
    return {r, g, b};
}

void Shape::loadVertices(const std::vector<float> &vertices){
    unsigned int numberVertices = vertices.size();
    float verticesArray[numberVertices];
    std::copy(vertices.begin(), vertices.end(), verticesArray);
    vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float));

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
    vb.sendData(verticesArray, Shape::DIMENSIONS * numberVertices * sizeof(float));

    VertexBufferLayout layout;
    layout.pushElement(GL_FLOAT, Shape::DIMENSIONS);
    va.addBuffer(vb, layout);
}

void Shape::loadIndices(const std::vector<unsigned int> &indices){
    unsigned int numberIndices = indices.size();
    unsigned int indicesArray[numberIndices];
    std::copy(indices.begin(), indices.end(), indicesArray);
    ib.sendData(indicesArray, numberIndices);
}

void Shape::loadIndicesLinesPointIndices(const std::vector<unsigned int> &indices){
    unsigned int sizeIndices = indices.size();
    unsigned int numberIndices = 2 * sizeIndices;
    unsigned int indicesArray[numberIndices];
    for (unsigned int i = 0; i < sizeIndices; i++){
        indicesArray[2 * i] = indices[i % sizeIndices];
        indicesArray[2 * i + 1] = indices[(i + 1) % sizeIndices];
    }
    ib.sendData(indicesArray, numberIndices);
}

void Shape::loadIndicesLinesPointIndicesOpen(const std::vector<unsigned int> &indices){
    unsigned int sizeIndices = indices.size() - 1;
    unsigned int numberIndices = 2 * sizeIndices;
    unsigned int indicesArray[numberIndices];
    for (unsigned int i = 0; i < sizeIndices; i++){
        indicesArray[2 * i] = indices[i];
        indicesArray[2 * i + 1] = indices[i + 1];
    }
    ib.sendData(indicesArray, numberIndices);
}

void Shape::loadIndicesDefault(unsigned int size, bool open){
    if (open){
        size--;
    }
    unsigned int numberIndices = 2 * size;
    unsigned int indicesArray[numberIndices];
    for (unsigned int i = 0; i < size - 1; i++){
        indicesArray[2 * i] = i;
        indicesArray[2 * i + 1] = i + 1;
    }
    indicesArray[numberIndices - 2] = size - 1;
    if (open){
        indicesArray[numberIndices - 1] = size;
    } else{
        indicesArray[numberIndices - 1] = 0;
    }
    ib.sendData(indicesArray, numberIndices);
}

void Shape::bindAll() const{
    vb.bind();
    va.bind();
    ib.bind();
}

unsigned int Shape::getNumberGeometricPrimitive(GeometricPrimitive primitive){
    if (primitive == GeometricPrimitive::Point || primitive == GeometricPrimitive::Polygon){
        return 1;
    } else if (primitive == GeometricPrimitive::Line ||
               primitive == GeometricPrimitive::LinePointOpen || primitive == GeometricPrimitive::LinePointClosed){
        return 2;
    } else if (primitive == GeometricPrimitive::Triangle){
        return 3;
    } else{
        LOG(LogLevel::ERROR) << "geometric primitive not implemented";
        return 0;
    }
}

//unsigned int Shape::getNumberIndicesOrder(IndicesOrder order){
//    if (order == IndicesOrder::Single){
//        return 1;
//    } else if (order == IndicesOrder::Double){
//        return 2;
//    } else if (order == IndicesOrder::Triple){
//        return 3;
//    } else{
//        LOG(LogLevel::ERROR) << "indices order not implemented";
//        return 0;
//    }
//}

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

LinesPointIndices::LinesPointIndices(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndicesLinesPointIndices(indices);
}

LinesPointIndices::LinesPointIndices(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndicesLinesPointIndices(indices);
}

LinesPointIndices::LinesPointIndices(const std::vector<Vector2f>& vertices){
    loadVertices2D(vertices);
    loadIndicesDefault(vertices.size(), false);
}

LinesPointIndices::~LinesPointIndices() {}

void LinesPointIndices::draw() const{
    bindAll();
    glDrawElements(GL_LINES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}

LinesPointIndicesOpen::LinesPointIndicesOpen(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndicesLinesPointIndicesOpen(indices);
}

LinesPointIndicesOpen::LinesPointIndicesOpen(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndicesLinesPointIndicesOpen(indices);
}

LinesPointIndicesOpen::LinesPointIndicesOpen(const std::vector<Vector2f>& vertices){
    loadVertices2D(vertices);
    loadIndicesDefault(vertices.size(), true);
}

LinesPointIndicesOpen::~LinesPointIndicesOpen() {}

void LinesPointIndicesOpen::draw() const{
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

Figure::Figure(const std::vector<float> &vertices, const std::vector<unsigned int> &indices){
    loadVertices(vertices);
    loadIndices(indices);
}

Figure::Figure(const std::vector<Vector2f> &vertices, const std::vector<unsigned int> &indices){
    loadVertices2D(vertices);
    loadIndices(indices);
}

Figure::~Figure() {}

void Figure::draw() const{
    bindAll();
    glDrawElements(GL_POLYGON, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}
