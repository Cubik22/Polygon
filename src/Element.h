#ifndef ELEMENT_H
#define ELEMENT_H

#include "Polygon.h"
#include "Network.h"

class Element{

public:
    Element(const Polygon& _poly);

    const std::vector<std::vector<unsigned int>*>& createElement();

    const std::vector<Vector2f>& getPoints();
    const std::vector<std::vector<unsigned int>*>& getPolygonsIndices();

    static const double TOLERANCE;

private:
    Polygon poly;
    std::vector<Vector2f> points;
    std::vector<std::vector<unsigned int>*> polygonsIndices;

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

    void createBoundingBox();
    void addExtraPoints();

    void tryAdding(const Vector2f& vector, Node* start, bool forward, bool bigger, bool x);
    void add(Node* start, Node* create, bool forward, bool bigger, bool x);

    void createMainPolygonUpdated(Node* start);
    void createBorderPolygon(Node* start, Node* begin);

    Node* findStartNode(Node* start);

    int getOneEqual(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b);

    // Debug
    void printArray(const std::vector<unsigned int>& a);
};

#endif // ELEMENT_H
