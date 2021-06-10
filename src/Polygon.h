#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <memory>
#include "Logger.h"
#include "Intersector.h"
#include "Network.h"


class Polygon{

public:
    Polygon();
    Polygon(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    ~Polygon();

    Polygon(const Polygon&)                 = delete;
    Polygon(Polygon&&) noexcept             = delete;
    Polygon& operator=(const Polygon&)      = delete;
    Polygon& operator=(Polygon&&) noexcept  = delete;

    void setBody(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices);
    void setSegment(const Vector2f& _p1, const Vector2f& _p2);

    const Vector2f& getPoint(unsigned int index) const;
    const std::vector<Vector2f>& getPoints() const;
    const std::vector<unsigned int>& getIndices() const;
    std::vector<Vector2f> getSegment() const;
    unsigned int getNumberIndices() const;
    const Node* getStartNode() const;
    unsigned int getNumberIntersections() const;

    void printVertices              (LogLevel level = LogLevel::INFO) const;
    void printIndices               (LogLevel level = LogLevel::INFO) const;
    void printNetwork               (LogLevel level = LogLevel::INFO) const;
    void printNetworkWithCoordinates(LogLevel level = LogLevel::INFO) const;

    void createNetwork(bool addSegmentPoints = false);

    void createNetworkMesh(std::vector<Vector2f>& extraPoint);

    void deleteStartNode();

    std::vector<std::shared_ptr<std::vector<unsigned int>>> cut();

    void cutIndices(std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices);

    void cutInsideOutside(std::vector<std::shared_ptr<std::vector<unsigned int>>>& insideIndices,
                          std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsideIndices);

    void cutInsideOutsideConcave(std::vector<std::shared_ptr<std::vector<unsigned int>>>& insideIndices,
                                 std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsideIndices,
                                 const std::vector<Vector2f>& concaveVertices, const std::vector<unsigned int>& concaveIndices,
                                 RelativePosition relativePosition);

    float getWidth()  const;
    float getHeight() const;
    float getXMin()   const;
    float getYMin()   const;

    static const double BIG_DOUBLE;

    // this function calculate the polygon area using Gauss
    static double CalculateArea(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);
    // with default indices
    static double CalculateArea(const std::vector<Vector2f>& vertices);

    static std::vector<Vector2f> translateVertices(const std::vector<Vector2f>& vertices, float diffX, float diffY);
    static std::shared_ptr<std::vector<Vector2f>> translateVerticesPointer(const std::vector<Vector2f>& vertices, float diffX, float diffY);

    static std::vector<Vector2f> scaleVertices(const std::vector<Vector2f>& vertices,
                                               float originalWidth, float originalHeight, float newWidth, float newHeight);

    static void scaleXYMin(float* const originalXMin, float* const originalYMin,
                           float originalWidth, float originalHeight, float newWidth, float newHeight);

    static void createBoundingBoxVariables(const std::vector<Vector2f>& vertices, float& width, float& height, float& xMin, float& yMin);

    static bool isPointBoundaryConcavePolygon(const Vector2f& point,
                                              const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);

    static bool isPointInsideConcavePolygon(const Vector2f& point,
                                            const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices);

private:
    std::vector<Vector2f> points;
    std::vector<unsigned int> indices;
    Node* startNode;
    Node* firstNode;
    RelativePosition orientation;
    Vector2f p1;
    Vector2f p2;
    unsigned int numberIntersections;

    float width;
    float height;
    float xMin;
    float yMin;

    bool alsoSegmentPoints;

    // simply check if the number of points and indices is greater than 1
    void checkEnoughPointIndices() const;

    bool checkIsPointIntersection(const Node* node) const;

    // return a node from where it can be calculated inside/outside concave polygon,
    // return the same node as input if all points are on boundary
    const Node* getNodeNotBoundary(const Node* node);

    void addSegmentPointsToNetwork();

    const Node* getNextNotBetween(const Node* node, bool upDirection);
    const Node* getNextNotBetween(const Node* node, bool upDirection, std::vector<unsigned int>& indicesPoli);

    // this function calculate the relative orientation of the polygon with the segment
    // it is used in getNextIntersection to, given an intersection node, find wich one of the two intersection nodes closer (up and down)
    // can be reached with a line inside the segment
    void calculateOrientation();
    //RelativePosition calculateOrientationFromNode(Node* node);

    // as stated before, this function only takes intersection nodes as input
    const Node* getNextIntersection(const Node* node);

    const Node* getNextIntersectionAdding(const Node* node, std::vector<unsigned int>& indicesPoli);

    // this function is used to order the intersection nodes, from the one the segment touches last to the one it touches first
    void sortIntersectionsNetwork(const std::vector<Node*>& nodes);

    // this is the function that will be recursively called when cutting the polygon
    // the first parameter is the node we currently are at when cutting the polygon
    // the second parameter is the initial node of the small polygon we are considering
    // node == initialNode if we are creating a new small polygon (maby the function name should be changed)
    // relative position here is different from the RelativePosition used in orientation
    // this one specify if the small polygon is above or below the segment, obiously is relative,
    // but if always the same convention is applied it works
    // the fourth paramter is a reference to the particular set of indices of the small polygon, empty if the small polygon is just created
    // the last one is the reference to the array of indices of all small polygon, so when a new small polygon is created
    // we can add his new array of indices to the big array of arrays
    void continueSmallPolygon(const Node* node, const Node* initialNode, std::vector<unsigned int>& indicesPoli,
                              std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices);

    // this is the same but is used when we want to distinguish between polygons up or below segment
    void continueSmallPolygonInsideOutside(const Node* node, const Node* initialNode,
        std::shared_ptr<std::vector<unsigned int>> indicesPoliPoi,
        std::vector<std::shared_ptr<std::vector<unsigned int>>>& insidePolygonsIndices,
        std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsidePolygonsIndices);

    void continueSmallPolygonInsideOutsideConcave(const Node* node, const Node* initialNode,
        std::shared_ptr<std::vector<unsigned int>> indicesPoliPoi,
        std::vector<std::shared_ptr<std::vector<unsigned int>>>& insidePolygonsIndices,
        std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsidePolygonsIndices,
        const std::vector<Vector2f>& concaveVertices, const std::vector<unsigned int>& concaveIndices);
};

#endif //POLYGON_H
