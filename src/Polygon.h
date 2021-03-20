#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "Intersector.h"
#include "Network.h"


class Polygon{

public:
    Polygon();
    Polygon(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    ~Polygon();

    void setBody(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    void setSegment(const Vector2f& _p1, const Vector2f& _p2);

    const Vector2f& getPoint(unsigned int index) const;
    const std::vector<Vector2f>& getPoints() const;
    const std::vector<unsigned int>& getIndices() const;
    unsigned int getNumberIndices() const;
    const Node* getStartNode() const;
    unsigned int getNumberIntersections() const;
    void printVertices() const;
    void printIndices() const;

    void createNetwork();
    std::vector<std::vector<unsigned int>*> cut();

    static const double BIG_DOUBLE;

private:
    std::vector<Vector2f> points;
    std::vector<unsigned int> indices;
    Node* startNode;
    RelativePosition orientation;
    Vector2f p1;
    Vector2f p2;
    unsigned int numberIntersections;

    void calculateOrientation();
    const Node* getNextIntersection(const Node* node);
    void sortIntersectionsNetwork(const std::vector<Node*>& nodes);
    void continueSmallPolygon(const Node* node, const Node* initialNode, RelativePosition relativePosition,
                              std::vector<unsigned int>& indicesPoli, std::vector<std::vector<unsigned int>*>& polygonsIndices);
};

#endif //POLYGON_H
