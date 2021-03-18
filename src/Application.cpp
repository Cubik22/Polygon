#include "Application.h"
#include "Intersector.h"

Application::Application() {}

unsigned int Application::getNumberVertices() const{
    return vertices.size();
}

const std::vector<Vector2f> Application::getVertices() const{
    return vertices;
}

const std::vector<unsigned int> Application::getIndices() const{
    return indices;
}

bool Application::addVertex(const Vector2f &vertex){
    unsigned int numberVertices = getNumberVertices();
    if (numberVertices >= 2){
        Intersector inter;
        inter.setSegment1(vertex, vertices[numberVertices - 1]);
        for (unsigned int i = 0; i < numberVertices - 1; i++){
            inter.setSegment2(vertices[i], vertices[i + 1]);
            IntersectionType type = inter.calculateIntersection();
            if (type == IntersectionType::InsideSegment){
                return false;
            }
        }
    }
    vertices.push_back(vertex);
    if (numberVertices >= 1){
        indices.push_back(numberVertices - 1);
        indices.push_back(numberVertices);
    }
    return true;
}

void Application::closePolygon(){
    indices.push_back(vertices.size() - 1);
    indices.push_back(0);
}

void Application::removeLastVertex(){
    unsigned int numberVertices = getNumberVertices();
    if (numberVertices > 0){
        vertices.pop_back();
        if (numberVertices >= 2){
            indices.pop_back();
            indices.pop_back();
        }
    }
}
