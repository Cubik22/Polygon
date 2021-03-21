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

    // this function calculate the relative orientation of the polygon with the segment
    // it is used in getNextIntersection to, given an intersection node, find wich one of the two intersection nodes closer (up and down)
    // can be reached with a line inside the segment
    void calculateOrientation();
    // as stated before, this function only takes intersection nodes as input
    const Node* getNextIntersection(const Node* node);
    // this function is used to order the intersection nodes, from the one the segment touches last to the one it touches first
    void sortIntersectionsNetwork(const std::vector<Node*>& nodes);
    // this is the function that will be recursively called when cutting the polygon
    // the first parameter is the node we currently are at when cutting the polygon
    // the second parameter is the initial node of the small polygon we are considering
    // node == initialNode if we are creating a new small polygon (maby the function name should be changed)
    // relative position here is different from the RelativePosition used in orientation
    // this one specify if the small polygon is above or below the segment, obiously is relative, but if always the same convention is applied it works
    // the fourth paramter is a reference to the particular set of indices of the small polygon, empty if the small polygon is just created
    // the last one is the reference to the array of indices of all small polygon, so when a new small polygon is created
    // we can add his new array of indices to the big array of arrays
    void continueSmallPolygon(const Node* node, const Node* initialNode, RelativePosition relativePosition,
                              std::vector<unsigned int>& indicesPoli, std::vector<std::vector<unsigned int>*>& polygonsIndices);
};

#endif //POLYGON_H
