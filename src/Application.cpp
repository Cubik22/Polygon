#include "Application.h"
#include "Intersector.h"
#include <fstream>
#include <sstream>


Application::Application() {}

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
    if (numberVertices >= 1){
        indices.push_back(numberVertices - 1);
        indices.push_back(numberVertices);
    }
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

const Polygon& Application::getPolygon() const{
    return mainPolygon;
}

void Application::createMainPolygon(){
    std::vector<unsigned int> listIndices;
    for (unsigned int i = 0; i < indices.size() / 2; i++){
        listIndices.push_back(indices[2 * i]);
    }
    mainPolygon.setBody(vertices, listIndices);
}


void Application::cutMainPolygon(){
    mainPolygon.setSegment(segmentPoints[0], segmentPoints[1]);
    mainPolygon.createNetwork();
    //Network::printNetwork(mainPolygon.getStartNode());
    polygonsIndices = mainPolygon.cut();
    std::cout << "End of cutting\n";
}

const std::vector<std::vector<unsigned int>*>& Application::getPolygonsIndices() const{
    return polygonsIndices;
}

void Application::printVertices() const{
    std::cout << "Printing application vertices\n";
    for (unsigned int i = 0; i < vertices.size(); i++){
        std::cout << i << " x: " << vertices[i].x << " y: " << vertices[i].y << "\n";
    }
}

void Application::printIndices() const{
    std::cout << "Printing application indices\n";
    for (unsigned int i = 0; i < indices.size(); i++){
        std::cout << i << " " << indices[i] << "\n";
    }
}

bool Application::loadVerticesFromFile(const std::string& fileName){
    std::cout << "Loading vertices from " << fileName << "\n";
    unsigned int numberVertices;
    std::ifstream file;
    std::string line;
    std::stringstream convert;
    file.open(fileName);
    if (file.fail()){
        std::cerr << "ERROR: failed to open file " << fileName << "\n";
        return false;
    }
    getline(file, line);
    convert.str(line);
    convert >> numberVertices;
    vertices.reserve(numberVertices);
    indices.reserve(2 * numberVertices);
    for (unsigned int i = 0; i < numberVertices; i++){
        float x, y;
        getline(file, line);
        convert.clear();
        convert.str(line);
        convert >> x;
        convert >> y;
        //std::cout << "x: " << x << " y: " << y << "\n";
        vertices.emplace_back(x, y);
        indices.push_back(i % numberVertices);
        indices.push_back((i + 1) % numberVertices);
    }
    file.close();
    std::cout << "Vertices correctly loaded\n";
    return true;
}

void Application::saveVerticesToFile(const std::string& fileName) const{
    std::cout << "Saving vertices to " << fileName << "\n";
    unsigned int numberVertices = getNumberVertices();
    std::ofstream file;
    file.open(fileName);
    if (file.fail()){
        std::cerr << "ERROR: failed to open file " << fileName << "\n";
        return;
    }
    file << numberVertices << "\n";
    for (unsigned int i = 0; i < numberVertices; i++){
        file << vertices[i].x << " " << vertices[i].y << "\n";
    }
    file.close();
    std::cout << "Vertices correctly saved\n";
}
