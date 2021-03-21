#ifndef APPLICATION_H
#define APPLICATION_H

#include "Polygon.h"
#include "Vector2f.h"
#include <vector>
#include <iostream>


class Application{

public:
    Application();

    const std::vector<unsigned int> getIndices() const;
    const std::vector<Vector2f> getVertices() const;
    unsigned int getNumberVertices() const;
    bool addVertex(const Vector2f& vertex);
    void removeLastVertex();
    bool closePolygon();

    bool addSegmentPoint(const Vector2f& point);
    bool removeSegmentPoint();
    const std::vector<Vector2f>& getSegmentPoints() const;
    unsigned int getSegmentSize() const;
    void createSegment();

    const Polygon& getPolygon() const;
    void createMainPolygon();
    void cutMainPolygon();
    const std::vector<std::vector<unsigned int>*>& getPolygonsIndices() const;

    bool isVerticesIndicesLoaded() const;
    bool isSegmentLoaded() const;

    void printVertices() const;
    void printIndices() const;

    void clear();

    // return: positve number if read succesfuly a positive number, 0 or negative if number is negative or some error occurred
    int getNumberVerticesFromFile(const std::string& fileName);
    // return: 0 file not found 1 search found -1 search not found
    int searchInFile(const std::string& fileName, const std::string& search, bool drawError = true);
    // return: 1 if loaded succesfuly, 0 or negative if some error occurred
    int loadSegmentFromFile(const std::string& fileName);
    // return: 1 if loaded succesfuly, 0 or negative if some error occurred
    int loadVerticesFromFile(const std::string& fileName, unsigned int numberVertices, bool loadIndices = false);
    // saves polygon to file (relative path is from the exectuable)
    void savePolygonToFile(const std::string& fileName) const;

private:
    bool verticesIndicesLoaded;
    bool segmentLoaded;
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    Polygon mainPolygon;
    std::vector<Vector2f> segmentPoints;
    std::vector<std::vector<unsigned int>*> polygonsIndices;

    // return: 0 file not found 1 search found -1 search not found
    int openFileAndSearch(std::ifstream& file, const std::string fileName, const std::string& search, bool drawError = true);
};

#endif // APPLICATION_H
