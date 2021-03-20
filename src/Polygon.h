#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <list>
#include <iterator>
#include "Intersector.h"
#include "Network.h"

class Polygon{
private:
    std::vector<Vector2f> points;
    std::vector<unsigned int> indices;
    Node* startNode;
    RelativePosition orientation;
    Vector2f p1;
    Vector2f p2;
    void calculateOrientation();
    const Node* getNextIntersection(const Node* node);
    void sortIntersectionsNetwork(const std::vector<Node*>& nodes);
    void createSmallPolygon(const Node* node, std::vector<std::vector<unsigned int>*>& polygonsIndices);
    void continueSmallPolygon(const Node* node, const Node* initialNode, RelativePosition relativePosition,
                              std::vector<unsigned int>& indicesPoli, std::vector<std::vector<unsigned int>*>& polygonsIndices);
public:
    Polygon();
    ~Polygon();
    Polygon(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    void setBody(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    void setSegment(const Vector2f& _p1, const Vector2f& _p2);
    const Vector2f& getPoint(unsigned int index) const;
    unsigned int getNumberIndices() const;
    const std::vector<Vector2f>& getPoints() const;
    const std::vector<unsigned int>& getIndices() const;
    //void addPointAfter(const Vector2f& point, unsigned int previous);
    //void addPointAt(const Vector2f& point, std::list<unsigned int>::const_iterator& it);
    void createNetwork();
    std::vector<std::vector<unsigned int>*> cut();
    const Node* getStartNode() const;
    void printVertices() const;
    void printIndices() const;
};

#endif //POLYGON_H
