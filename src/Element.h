#ifndef ELEMENT_H
#define ELEMENT_H

#include "Polygon.h"
#include "Network.h"


class Element{

public:
    Element(const Polygon& _poly);

    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& createElement();

    const std::vector<Vector2f>& getPoints() const;
    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& getPolygonsIndices() const;

    float getWidth() const;
    float getHeight() const;

    float getXMin() const;
    float getYMin() const;

    static const double TOLERANCE;

private:
    std::vector<Vector2f> points;
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<std::vector<unsigned int>>> polygonsIndices;

    unsigned int numberStartIndices;
    unsigned int numberAddedVertices;

    bool created;

    float top;
    float bottom;
    float right;
    float left;

    float width;
    float height;
    float xMin;
    float yMin;

    Node* topRight;
    Node* topLeft;
    Node* bottomRight;
    Node* bottomLeft;

    // NOTE: node->touched in this context means that the node is on the border and not in the polygon

    void createBoundingBox();
    void addExtraPoints();

    // used for trying to add opposite points (and nodes)
    // these are added just if there is not already a point
    // when adding, it also check whether to add them to border indices or polygon indices
    void tryAdding(const Vector2f& vector, Node* start, bool forward, bool bigger, bool x);
    // used for adding normal nodes
    void add(Node* start, Node* create, bool forward, bool bigger, bool x);

    // create indices of main polygon with extra points added
    void createMainPolygonUpdated(Node* start);
    // create all other border polygons
    void createBorderPolygon(Node* start, Node* begin);

    // find the first node in order to start createBorderPolygon
    // start node should be a polygon node on the border and not between two other polygon nodes on the border
    Node* findStartNode(Node* start);

    // return the unsigned int contained in both vectors if it finds one
    // return a negative number if it does not find it
    int getOneEqual(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b);

    // Debug
    void printDebugArray(const std::vector<unsigned int>& a);
};

#endif // ELEMENT_H
