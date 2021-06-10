#include "Application.h"
#include "Intersector.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

Application::Application() : verticesIndicesLoaded{false}, segmentLoaded{false} {}

const std::vector<unsigned int> Application::getIndices() const{
    return indices;
}

const std::vector<Vector2f> Application::getVertices() const{
    return vertices;
}

unsigned int Application::getNumberVertices() const{
    return vertices.size();
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
    indices.push_back(numberVertices);
    return true;
}

void Application::removeLastVertex(){
    unsigned int numberVertices = getNumberVertices();
    if (numberVertices > 0){
        vertices.pop_back();
        indices.pop_back();
    }
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
    verticesIndicesLoaded = true;
    return true;
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

void Application::createSegment(){
    segmentLoaded = true;
}

const Polygon& Application::getPolygon() const{
    return mainPolygon;
}

void Application::createMainPolygon(){
    std::vector<unsigned int> listIndices;
    for (unsigned int i = 0; i < indices.size(); i++){
        listIndices.push_back(indices[i]);
    }
    mainPolygon.setBody(vertices, listIndices);
}


void Application::cutMainPolygon(bool alsoSegmentPoints){
    cutted = true;
    mainPolygon.setSegment(segmentPoints[0], segmentPoints[1]);
    mainPolygon.createNetwork(alsoSegmentPoints);
    //Network::printNetwork(mainPolygon.getStartNode());
    polygonsIndices = mainPolygon.cut();
    LOG(LogLevel::INFO) << "End of cutting";
}

const std::vector<std::shared_ptr<std::vector<unsigned int>>>& Application::getPolygonsIndices() const{
    return polygonsIndices;
}

void Application::setVerticesIndicesLoaded(){
    verticesIndicesLoaded = true;
}

void Application::setSegmentLoaded(){
    segmentLoaded = true;
}

bool Application::isVerticesIndicesLoaded() const{
    return verticesIndicesLoaded;
}

bool Application::isSegmentLoaded() const{
    return segmentLoaded;
}

void Application::printVertices() const{
    LOG(LogLevel::INFO) << "Printing application vertices";
    for (unsigned int i = 0; i < vertices.size(); i++){
        LOG(LogLevel::INFO) << i << " x: " << vertices[i].x << " y: " << vertices[i].y;
    }
}

void Application::printIndices() const{
    LOG(LogLevel::INFO) << "Printing application indices";
    for (unsigned int i = 0; i < indices.size(); i++){
        LOG(LogLevel::INFO) << i << " " << indices[i];
    }
}

void Application::clear(){
    vertices.clear();
    indices.clear();
    //mainPolygon = Polygon();
    mainPolygon.deleteStartNode();
    polygonsIndices.clear();
    segmentPoints.clear();
    verticesIndicesLoaded = false;
    segmentLoaded = false;
    LOG(LogLevel::DEBUG) << "Application cleared";
}

std::vector<unsigned int>& Application::getIndicesForLoading(){
    return indices;
}

std::vector<Vector2f>& Application::getVerticesForLoading(){
    return vertices;
}

std::vector<Vector2f>& Application::getSegmentForLoading(){
    return segmentPoints;
}

const std::vector<Vector2f>& Application::getBoxVertices() const{
    return boxVertices;
}

const std::vector<Vector2f>& Application::getDefautVertices() const{
    return defaultVertices;
}

const std::vector<Vector2f>& Application::getBox(unsigned int position) const{
    if (boxes.size() > position){
        return boxes[position];
    } else{
        LOG(LogLevel::ERROR) << "vector out of bound in Application::getBox";
        return boxes[0];
    }
}

const float& Application::getWidth() const{
    return width;
}

const float& Application::getHeight() const{
    return height;
}

void Application::createBox(){
    unsigned int numberVertices = getNumberVertices();
    float right = vertices[0].x;
    float left = vertices[0].x;
    float top = vertices[0].y;
    float bottom = vertices[0].y;
    for (unsigned int i = 1; i < numberVertices; i++){
        const float& x = vertices[i].x;
        const float& y = vertices[i].y;
        if (x > right){
            right = x;
        } else if (x < left){
            left = x;
        }
        if (y > top){
            top = y;
        } else if (y < bottom){
            bottom = y;
        }
    }

    boxVertices = {
        {left, bottom},
        {right, bottom},
        {right, top},
        {left, top}
    };
    width = right - left;
    height = top - bottom;
    xMin = left;
    yMin = bottom;

    for (unsigned int i = 0; i < numberVertices; i++){
        float y = vertices[i].y - bottom;
        float x = vertices[i].x - left;
        //defaultVertices.emplace_back((2 * x / width) - 1, (2 * y / height) - 1);
        defaultVertices.emplace_back(x / width, y / height);
    }
}

void Application::copyPolygon(float x, float y, float width, float height){
    unsigned int numberVertices = getNumberVertices();
    std::vector<Vector2f> newPolygon;
    newPolygon.reserve(numberVertices);

    for (unsigned int i = 0; i < numberVertices; i++){
        newPolygon.emplace_back(defaultVertices[i].x * width + x, defaultVertices[i].y * height + y);
    }
    boxes.push_back(newPolygon);
}

void Application::movePolygon(unsigned int number, float x, float y){
    if (number >= boxes.size()){
        return;
    }
    std::vector<Vector2f>& vert = boxes[number];
    unsigned int numberVertices = getNumberVertices();

    for (unsigned int i = 0; i < numberVertices; i++){
        vert[i].x += x;
        vert[i].y += y;
    }
}

void Application::createGrid(float xMin, float yMin, float width, float height, unsigned int numberX, unsigned int numberY){
    //unsigned int numberVertices = getNumberVertices();
    for (unsigned int x = 0; x < numberX; x++){
        for (unsigned int y = 0; y < numberY; y++){
            copyPolygon(xMin + width * x, yMin + height * y, width, height);
        }
    }
}
