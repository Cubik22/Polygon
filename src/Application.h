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

    const Polygon& getPolygon() const;
    void createMainPolygon();
    void cutMainPolygon();
    const std::vector<std::vector<unsigned int>*>& getPolygonsIndices() const;

    void printVertices() const;
    void printIndices() const;

    bool loadVerticesFromFile(const std::string& fileName);
    void saveVerticesToFile(const std::string& fileName) const;

private:
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    Polygon mainPolygon;
    std::vector<Vector2f> segmentPoints;
    std::vector<std::vector<unsigned int>*> polygonsIndices;
};

#endif // APPLICATION_H
