#include "Application.h"
#include "Intersector.h"
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

bool Application::isVerticesIndicesLoaded() const{
    return verticesIndicesLoaded;
}

bool Application::isSegmentLoaded() const{
    return segmentLoaded;
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

void Application::clear(){
    vertices.clear();
    indices.clear();
    mainPolygon = Polygon();
    polygonsIndices.clear();
    verticesIndicesLoaded = false;
    segmentLoaded = false;
    std::cout << "Application cleared\n";
}

void Application::savePolygonToFile(const std::string& fileName) const{
    std::cout << "Saving polygon to " << fileName << "\n";
    unsigned int numberVertices = getNumberVertices();
    std::ofstream file;
    file.open(fileName);
    if (file.fail()){
        std::cerr << "ERROR: failed to open file " << fileName << "\n";
        return;
    }
    file << "# segment\n";
    file << segmentPoints[0].x << " " << segmentPoints[1].y << "\n";
    file << segmentPoints[1].x << " " << segmentPoints[1].y << "\n";
    file << "# number of vertices\n";
    file << numberVertices << "\n";
    file << "# indices\n";
    for (unsigned int i = 0; i < numberVertices - 1; i++){
        file << indices[2 * i] << " ";
    }
    file << indices[2 * (numberVertices - 1)] << "\n";
    file << "# vertices\n";
    for (unsigned int i = 0; i < numberVertices; i++){
        file << vertices[i].x << " " << vertices[i].y << "\n";
    }
    file.close();
    std::cout << "Polygon correctly saved\n";
}

int Application::searchInFile(const std::string& fileName, const std::string& search, bool drawError){
    std::ifstream file;
    int found = openFileAndSearch(file, fileName, search, drawError);
    file.close();
    return found;
}

int Application::getNumberVerticesFromFile(const std::string& fileName){
    std::ifstream file;
    int found = openFileAndSearch(file, fileName, "number of vertices");
    if (found != 1){
        return found;
    }
    std::string line;
    std::stringstream convert;
    int numberVertices = 0;
    getline(file, line);
    convert.str(line);
    convert >> numberVertices;
    file.close();
    if (numberVertices <= 0 || convert.fail()){
        std::cout << "ERROR: number of vertices should be a number greather than 0\n";
        return -2;
    }
    return numberVertices;
}

int Application::loadSegmentFromFile(const std::string& fileName){
    std::ifstream file;
    int found = openFileAndSearch(file, fileName, "segment");
    if (found != 1){
        return found;
    }
    std::string line;
    std::stringstream convert;
    for (unsigned int i = 0; i < 2; i++){
        getline(file, line);
        float x, y;
        convert.str(line);
        convert >> x >> y;
        if (convert.fail()){
            std::cerr << "ERROR: problems when reading segment\n";
            std::cerr << "Line: " << line << "\n";
            segmentPoints.clear();
            return -3;
        }
        convert.clear();
        segmentPoints.emplace_back(x, y);
    }
    file.close();
    std::cout << "Correctly loaded segment from " << fileName << "\n";
    segmentLoaded = true;
    return 1;
}

int Application::loadVerticesFromFile(const std::string& fileName, unsigned int numberVertices, bool loadIndices){
    if (numberVertices == 0){
        std::cerr << "ERROR: number of vertices should be a number greather than 0\n";
        return -2;
    }
    std::ifstream file;
    openFileAndSearch(file, fileName, "vertices");
    std::string line;
    std::stringstream convert;
    vertices.reserve(numberVertices);
    indices.reserve(2 * numberVertices);
    for (unsigned int i = 0; i < numberVertices; i++){
        getline(file, line);
        float x, y;
        convert.str(line);
        convert >> x >> y;
        if (convert.fail()){
            std::cerr << "ERROR: problems when reading vertices\n";
            std::cerr << "Line: " << line << "\n";
            vertices.clear();
            indices.clear();
            return -3;
        }
        vertices.emplace_back(x, y);
        convert.clear();
        if (!loadIndices){
            indices.push_back(i % numberVertices);
            indices.push_back((i + 1) % numberVertices);
        }
    }
    file.close();
    std::cout << "Correctly loaded vertices from " << fileName << "\n";

    if (!loadIndices){
        verticesIndicesLoaded = true;
        return 1;
    }
    openFileAndSearch(file, fileName, "indices");
    getline(file, line);
    unsigned int index;
    unsigned int previousIndex;
    unsigned int firstIndex;
    convert.str(line);
    convert >> previousIndex;
    if (convert.fail()){
        std::cerr << "ERROR: problems when reading indices\n";
        std::cerr << "Line: " << line << "\n";
        vertices.clear();
        return -3;
    }
    firstIndex = previousIndex;
    for (unsigned int i = 0; i < numberVertices - 1; i++){
        convert >> index;
        if (convert.fail()){
            std::cerr << "ERROR: problems when reading indices\n";
            std::cerr << "Line: " << line << "\n";
            vertices.clear();
            indices.clear();
            return -3;
        }
        indices.push_back(previousIndex);
        indices.push_back(index);
        previousIndex = index;
    }
    indices.push_back(index);
    indices.push_back(firstIndex);
    file.close();
    std::cout << "Correctly loaded indices from " << fileName << "\n";
    verticesIndicesLoaded = true;
    return 1;
}

int Application::openFileAndSearch(std::ifstream& file, const std::string fileName, const std::string& search, bool drawError){
    file.open(fileName);
    if (file.fail()){
        if (drawError){
            std::cerr << "ERROR: failed to open file " << fileName << "\n";
        }
        return 0;
    }
    std::string searchLine = "# " + search;
    std::string line;
    getline(file, line);
    while (line != searchLine){
        getline(file, line);
        if (file.eof()){
            if (drawError){
                std::cerr << "ERROR: failed to load " << search << "\n";
            }
            return -1;
        }
    }
    return 1;
}
