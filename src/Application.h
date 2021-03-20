#ifndef APPLICATION_H
#define APPLICATION_H

#include "Polygon.h"
#include "Vector2f.h"
#include <vector>
#include <iostream>

class Application{
private:
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    Polygon mainPolygon;
    std::vector<std::vector<unsigned int>*> polygonsIndices;
    std::vector<Vector2f> segmentPoints;
public:
    Application();
    unsigned int getNumberVertices() const;
    const std::vector<Vector2f> getVertices() const;
    const std::vector<unsigned int> getIndices() const;
    bool addVertex(const Vector2f& vertex);
    bool closePolygon();
    void removeLastVertex();
    void createMainPolygon();
    const Polygon& getPolygon() const;
    bool addSegmentPoint(const Vector2f& point);
    bool removeSegmentPoint();
    const std::vector<Vector2f>& getSegmentPoints() const;
    unsigned int getSegmentSize() const;
    void cutMainPolygon(){
        mainPolygon.setSegment(segmentPoints[0], segmentPoints[1]);
        mainPolygon.createNetwork();
        //Network::printNetwork(mainPolygon.getStartNode());
        polygonsIndices = mainPolygon.cut();
        std::cout << "fine taglio\n";
    }
    const std::vector<std::vector<unsigned int>*>& getPolygonsIndices() const{
        return polygonsIndices;
    }
};

#endif // APPLICATION_H
