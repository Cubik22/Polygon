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

    void setVerticesIndicesLoaded();
    void setSegmentLoaded();

    bool isVerticesIndicesLoaded() const;
    bool isSegmentLoaded() const;

    void printVertices() const;
    void printIndices() const;

    void clear();

    // used when loading from file
    std::vector<unsigned int>& getIndicesForLoading();
    std::vector<Vector2f>& getVerticesForLoading();
    std::vector<Vector2f>& getSegmentForLoading();

    const std::vector<Vector2f>& getBoxVertices() const;
    void createBox();

private:
    bool verticesIndicesLoaded;
    bool segmentLoaded;
    bool cutted = false;
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    Polygon mainPolygon;
    std::vector<Vector2f> segmentPoints;
    std::vector<std::vector<unsigned int>*> polygonsIndices;

    std::vector<Vector2f> boxVertices;
};

#endif // APPLICATION_H
