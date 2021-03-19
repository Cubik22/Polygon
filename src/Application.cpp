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

bool Application::closePolygon(){
    unsigned int numberVertices = getNumberVertices();
    if (numberVertices < 3){
        return false;
    }
    Intersector inter;
    inter.setSegment1(vertices[numberVertices - 1], vertices[0]);
    for (unsigned int i = 0; i < numberVertices - 1; i++){
        inter.setSegment2(vertices[i], vertices[i + 1]);
        IntersectionType type = inter.calculateIntersection();
        if (type == IntersectionType::InsideSegment){
            return false;
        }
    }
    indices.push_back(vertices.size() - 1);
    indices.push_back(0);
    return true;
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

void Application::createMainPolygon(){
    std::vector<unsigned int> listIndices;
    for (unsigned int i = 0; i < indices.size() / 2; i++){
        listIndices.push_back(indices[2 * i]);
    }
    mainPolygon.setBody(vertices, listIndices);
}

const Polygon& Application::getPolygon() const{
    return mainPolygon;
}

bool Application::addSegmentPoint(const Vector2f &point){
    if (segmentPoints.size() >= 2){
        return false;
    }
    segmentPoints.push_back(point);
    return true;
}

bool Application::removeSegmentPoint(){
    if (segmentPoints.size() > 0){
        segmentPoints.pop_back();
        return true;
    }
    return false;
}

const std::vector<Vector2f> &Application::getSegmentPoints() const{
    return segmentPoints;
}

unsigned int Application::getSegmentSize() const{
    return segmentPoints.size();
}
