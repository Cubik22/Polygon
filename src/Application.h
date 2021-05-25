#ifndef APPLICATION_H
#define APPLICATION_H

#include "Polygon.h"
#include "Vector2f.h"
#include <vector>
#include <iostream>


class Application{

public:
    Application();

    Application(const Application&)                 = delete;
    Application(Application&&) noexcept             = delete;
    Application& operator=(const Application&)      = delete;
    Application& operator=(Application&&) noexcept  = delete;

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
    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& getPolygonsIndices() const;

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
    const std::vector<Vector2f>& getDefautVertices() const;
    const std::vector<Vector2f>& getBox(unsigned int position) const;
    const float& getWidth() const;
    const float& getHeight() const;

    void createBox();

    void copyPolygon(float x, float y, float width, float height);
    void movePolygon(unsigned int number, float x, float y);

    void createGrid(float xMin, float yMin, float width, float height, unsigned int numberX, unsigned int numberY);

private:
    bool verticesIndicesLoaded;
    bool segmentLoaded;
    bool cutted = false;
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    Polygon mainPolygon;
    std::vector<Vector2f> segmentPoints;
    std::vector<std::shared_ptr<std::vector<unsigned int>>> polygonsIndices;

    std::vector<Vector2f> boxVertices;
    std::vector<Vector2f> defaultVertices;

    float width;
    float height;
    float xMin;
    float yMin;

    std::vector<std::vector<Vector2f>> boxes;
};

#endif // APPLICATION_H
