#include "Polygon.h"
#include "Logger.h"
#include <math.h>
#include <iostream>
#include <exception>

Polygon::Polygon() : startNode{nullptr} {}

Polygon::Polygon(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices) :
    points(_points), indices(_indices), startNode(nullptr) {

    Polygon::createBoundingBoxVariables(points, width, height, xMin, yMin);

    checkEnoughPointIndices();
}

Polygon::~Polygon(){
    deleteStartNode();
}

void Polygon::setBody(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices){
    points = _points;
    indices = _indices;
    Polygon::createBoundingBoxVariables(points, width, height, xMin, yMin);

    checkEnoughPointIndices();
}

void Polygon::setSegment(const Vector2f& _p1, const Vector2f& _p2){
    p1 = _p1;
    p2 = _p2;
}

const Vector2f& Polygon::getPoint(unsigned int index) const{
    if (index >= getNumberIndices()){
        LOG(LogLevel::ERROR) << "Polygon::getPoint: Array out of bound, returned point(0) instead";
        return points[0];
    }
    return points[index];
}

unsigned int Polygon::getNumberIndices() const{
    return indices.size();
}

const std::vector<Vector2f>& Polygon::getPoints() const{
    return points;
}

const std::vector<unsigned int>& Polygon::getIndices() const{
    return indices;
}

std::vector<Vector2f> Polygon::getSegment() const{
    return {p1, p2};
}

const Node* Polygon::getStartNode() const{
    return startNode;
}

unsigned int Polygon::getNumberIntersections() const{
    return numberIntersections;
}

void Polygon::printVertices(LogLevel level) const{
    LOG(level) << "Printing polygon vertices";
    for (unsigned int i = 0; i < points.size(); i++){
        LOG(level) << i << " x: " << points[i].x << " y: " << points[i].y;
    }
}

void Polygon::printIndices(LogLevel level) const{
    LOG(level) << "Printing polygon indices";
    for (unsigned int i = 0; i < indices.size(); i++){
        LOG(level) << i << " " << indices[i];
    }
}

void Polygon::printNetwork(LogLevel level) const{
    LOG(level) << "Printing polygon network";
    Node::PrintNetwork(startNode, level);
}

void Polygon::printNetworkWithCoordinates(LogLevel level) const{
    LOG(level) << "Printing polygon network with coordinates";
    const Node* start = startNode;
    const Node* node = start;
    unsigned int i = 0;
    do{
        if (node == nullptr){
            LOG(LogLevel::WARN) << i << ": is nullptr";
            return;
        }
        unsigned int index = node->getIndex();
        LOG(level) << i++ << ": " << index << " x: " << points[index].x << " y: " << points[index].y;
        node = node->next;
    } while (node != start);
}

void Polygon::createNetwork(){
    // stores the intersection Nodes in order to sort them after
    std::vector<Node*> unorderedIntersectionNodes;
    // intersector is used to find intersection points
    Intersector inter;
    // this is the segment, it will be treated as a line when computing intersection
    inter.setSegment1(p1, p2);

    numberIntersections = 0;

    // this vector is used to find the one of the two most external intersection node
    // precisely the last the segment line intersects
    // it is useful for helping the sorting  of the intersection nodes after
    Vector2f segment = p2 - p1;
    // in order to find this node we will compute the dot product with the segment
    // and one of point of the segment - the intersection node
    double minProduct = Polygon::BIG_DOUBLE;
    Node* minIntersectionNode = nullptr;

    // this stores the first normal node, in order to connect it in the end to the last node (the network is cyclical)
    firstNode = nullptr;
    // this stores the pointer to the temporary node we are considering
    Node* node = nullptr;
    // this stores the pointer to the previous node se we can connect it with the new one
    Node* previous = nullptr;

    // we store the node that could be intersection on vertex
    // this has to be done because when we find an intersection on vertex, it could be on the first or the second vertex
    // so we add the two vertices to this list of possible vertices then at the end we look for vertices whose previous's up are themselves
    // that is if the vertex before on the list point to the specific vertex then we know this last one is an intersection point
    std::vector<Node*> possibleOnVertex;

    unsigned int numberIndices = getNumberIndices();

    for (unsigned int i = 0; i < numberIndices; i++){
        node = new Node(indices[i]);
        // previous will be nullptr just the first time
        if (previous != nullptr){
            node->previous = previous;
            previous->next = node;
        } else{
            //LOG(LogLevel::DEBUG) << "first node setted";
            firstNode = node;
        }
        // here we set previous to node beacuse in the next iteration will be previous
        previous = node;
        // we calculate the intersection with the segment from the point to the next one
        inter.setSegment2(points[indices[i]], points[indices[(i + 1) % numberIndices]]);
        IntersectionType intersectionType = inter.calculateIntersection(true, false);

        // we separete the two cases when the intersection point is inside the segement
        // (if it is inside the segment intended as segment but on the vertex of the segment intended as line,
        // intersection point is FirstOnVertex (or SecondOnVertex if the two segment were swapped))
        bool isInsideSegment = intersectionType == IntersectionType::InsideSegment || intersectionType == IntersectionType::FirstOnVertex;
        bool isOnVertex = intersectionType == IntersectionType::BothOnVertex || intersectionType == IntersectionType::SecondOnVertex;

        if (isInsideSegment){
            const Vector2f intersectionPoint = inter.getIntersectionPoint();
            // we add the new point found to the points of the polygon
            points.push_back(intersectionPoint);
            // index of last point before intersection was numberIndices - 1 so the first intersection node starts from numberIndices
            //node = new Node(numberIndices + numberIntersections);
            node = new Node(points.size() - 1);
            // we add the new node found to the list of unordered intersection nodes
            unorderedIntersectionNodes.push_back(node);
            numberIntersections++;
            // again we connect it with the previous node because intersection nodes are connected both
            // like the normal nodes to previous and after
            // but are also connected between them with up and down
            node->previous = previous;
            previous->next = node;
            previous = node;

            // here is calculated the dot product in order to find the most outer intersection node
            double product = segment.dot(points[node->getIndex()] - p1);
            if (product < minProduct){
                minIntersectionNode = node;
                minProduct = product;
            }
        } else if (isOnVertex){
            possibleOnVertex.push_back(node);
        }
    }

    if (firstNode == nullptr){
        LOG(LogLevel::ERROR) << "first node is nullptr";
        return;
    }
    // finally we connect the first node with the last one and the cycle is closed
    firstNode->previous = previous;
    previous->next = firstNode;

    unsigned int numberPossibleOnVertex = possibleOnVertex.size();
    for (unsigned int i = 0; i < numberPossibleOnVertex; i++){
        if (possibleOnVertex[(i - 1) % numberPossibleOnVertex]->next == possibleOnVertex[i]){
            Node* nodeAdd = possibleOnVertex[i];

            if (checkIsPointIntersection(nodeAdd)){
                unorderedIntersectionNodes.push_back(nodeAdd);
                numberIntersections++;

                // here is calculated the dot product in order to find the most outer intersection node
                double product = segment.dot(points[nodeAdd->getIndex()] - p1);
                if (product < minProduct){
                    minIntersectionNode = nodeAdd;
                    minProduct = product;
                }
            }
        }
    }

    // here we handle the case where there are no intersections
    if (minIntersectionNode != nullptr){
        startNode = minIntersectionNode;
        sortIntersectionsNetwork(unorderedIntersectionNodes);

        calculateOrientation();

        LOG::NewLine(LogLevel::INFO);

    } else if (numberIntersections == 0){
        LOG(LogLevel::INFO) << "No intersections";
        startNode = firstNode;
    } else{
        LOG(LogLevel::ERROR) << "problems when setting startNode";
        startNode = firstNode;
    }
}

void Polygon::createNetworkMesh(std::vector<Vector2f>& extraPoint){
    if (extraPoint.size() != points.size()){
        LOG(LogLevel::WARN) << "points and extra points do not have the same size";
    }

    // stores the intersection Nodes in order to sort them after
    std::vector<Node*> unorderedIntersectionNodes;
    // intersector is used to find intersection points
    Intersector inter;
    // this is the segment, it will be treated as a line when computing intersection
    inter.setSegment1(p1, p2);

    numberIntersections = 0;

    // this vector is used to find the one of the two most external intersection node
    // precisely the last the segment line intersects
    // it is useful for helping the sorting  of the intersection nodes after
    Vector2f segment = p2 - p1;
    // in order to find this node we will compute the dot product with the segment
    // and one of point of the segment - the intersection node
    double minProduct = Polygon::BIG_DOUBLE;
    Node* minIntersectionNode = nullptr;

    // this stores the first normal node, in order to connect it in the end to the last node (the network is cyclical)
    firstNode = nullptr;
    // this stores the pointer to the temporary node we are considering
    Node* node = nullptr;
    // this stores the pointer to the previous node se we can connect it with the new one
    Node* previous = nullptr;

    // we store the node that could be intersection on vertex
    // this has to be done because when we find an intersection on vertex, it could be on the first or the second vertex
    // so we add the two vertices to this list of possible vertices then at the end we look for vertices whose previous's up are themselves
    // that is if the vertex before on the list point to the specific vertex then we know this last one is an intersection point
    std::vector<Node*> possibleOnVertex;

    unsigned int numberIndices = getNumberIndices();

    for (unsigned int i = 0; i < numberIndices; i++){
        node = new Node(indices[i]);
        // previous will be nullptr just the first time
        if (previous != nullptr){
            node->previous = previous;
            previous->next = node;
        } else{

            firstNode = node;
        }
        // here we set previous to node beacuse in the next iteration will be previous
        previous = node;
        // we calculate the intersection with the segment from the point to the next one
        inter.setSegment2(points[indices[i]], points[indices[(i + 1) % numberIndices]]);
        IntersectionType intersectionType = inter.calculateIntersection(true, false);

        // we separete the two cases when the intersection point is inside the segement
        // (if it is inside the segment intended as segment but on the vertex of the segment intended as line,
        // intersection point is FirstOnVertex (or SecondOnVertex if the two segment were swapped))
        bool isInsideSegment = intersectionType == IntersectionType::InsideSegment || intersectionType == IntersectionType::FirstOnVertex;
        bool isOnVertex = intersectionType == IntersectionType::BothOnVertex || intersectionType == IntersectionType::SecondOnVertex;

        if (isInsideSegment){
            // if it is inside the segment we search if the intersection point is already in the points
            // if it is we create a new Node pointing to that point, else we create also a new point

            const Vector2f intersectionPoint = inter.getIntersectionPoint();

            bool found = false;
            for (unsigned int l = 0; l < points.size(); l++){
                if ((intersectionPoint - points[l]).normSquared() < Vector2f::TOLERANCE){
                    node = new Node(l);
                    found = true;
                    break;
                }
            }
            if (!found){
                points.push_back(intersectionPoint);
                extraPoint.push_back(intersectionPoint);
                node = new Node(points.size() - 1);
            }

            // again we connect it with the previous node because intersection nodes are connected both
            // like the normal nodes to previous and after
            // but are also connected between them with up and down
            node->previous = previous;
            previous->next = node;
            previous = node;

            // we add the new node found to the list of unordered intersection nodes
            unorderedIntersectionNodes.push_back(node);
            numberIntersections++;

            // here is calculated the dot product in order to find the most outer intersection node
            double product = segment.dot(points[node->getIndex()] - p1);
            if (product < minProduct){
                minIntersectionNode = node;
                minProduct = product;
            }
        } else if (isOnVertex){
            possibleOnVertex.push_back(node);
        }
    }

    if (firstNode == nullptr){
        LOG(LogLevel::ERROR) << "first node is nullptr";
        return;
    }
    // finally we connect the first node with the last one and the cycle is closed
    firstNode->previous = previous;
    previous->next = firstNode;

    unsigned int numberPossibleOnVertex = possibleOnVertex.size();
    for (unsigned int i = 0; i < numberPossibleOnVertex; i++){
        if (possibleOnVertex[(i - 1) % numberPossibleOnVertex]->next == possibleOnVertex[i]){
            Node* nodeAdd = possibleOnVertex[i];

            if (checkIsPointIntersection(nodeAdd)){
                unorderedIntersectionNodes.push_back(nodeAdd);
                numberIntersections++;

                // here is calculated the dot product in order to find the most outer intersection node
                double product = segment.dot(points[nodeAdd->getIndex()] - p1);
                if (product < minProduct){
                    minIntersectionNode = nodeAdd;
                    minProduct = product;
                }
            }
        }
    }

    // orientation has to be set to positive because this mode only support anti-clock wise indices
    orientation = RelativePosition::Positive;

    // here we handle the case where there are no intersections
    if (minIntersectionNode != nullptr && numberIntersections > 1){
        startNode = minIntersectionNode;
        sortIntersectionsNetwork(unorderedIntersectionNodes);

        LOG::NewLine(LogLevel::INFO);

    } else if (numberIntersections == 0){
        LOG(LogLevel::INFO) << "No intersections";
        startNode = firstNode;
    } else if (numberIntersections == 1){
        // sometimes it happens that there is only one intersection, specifically when the points and indices are manually entered
        // this happens when the segment is tangent to the poligon, this case is handled as in the case when there are no intersections
        LOG(LogLevel::WARN) << "1 intersection";
        startNode = firstNode;
        numberIntersections = 0;
    } else{
        LOG(LogLevel::ERROR) << "problems when setting startNode, number of intersection: " << numberIntersections;
        startNode = firstNode;
    }
}

void Polygon::deleteStartNode(){
    if (startNode){
        Node::DeleteNetwork(startNode);
    } else{
        LOG(LogLevel::DEBUG) << "Polygon was not cutted";
    }
    LOG(LogLevel::DEBUG) << "Polygon deleted";
}

std::vector<std::shared_ptr<std::vector<unsigned int>>> Polygon::cut(){
    std::vector<std::shared_ptr<std::vector<unsigned int>>> polygonsIndices;
    if (numberIntersections > 0){
        startNode->touched = true;
        LOG(LogLevel::INFO) << startNode->getIndex() << " starting the cut";
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[0];
        continueSmallPolygon(startNode, startNode, indicesPoliCreation, polygonsIndices);
    } else{
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
    }
    return polygonsIndices;
}

void Polygon::cutIndices(std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices){
    if (startNode && numberIntersections > 0){
        startNode->touched = true;
        LOG(LogLevel::INFO) << startNode->getIndex() << " starting the cut";
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[polygonsIndices.size() - 1];
        continueSmallPolygon(startNode, startNode, indicesPoliCreation, polygonsIndices);
    } else{
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
    }
}

void Polygon::cutInsideOutside(std::vector<std::shared_ptr<std::vector<unsigned int>>>& insideIndices,
                               std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsideIndices){
    orientation = RelativePosition::Positive;
    if (startNode && numberIntersections > 0){
        if (!startNode || startNode == nullptr){
            LOG(LogLevel::ERROR) << "Start node is nullptr and number of intersection is > 0";
            return;
        }
        startNode->touched = true;
        LOG(LogLevel::INFO) << startNode->getIndex() << " starting the cut";
        continueSmallPolygonInsideOutside(startNode, startNode, nullptr, insideIndices, outsideIndices);
    } else{
        if (!firstNode || firstNode == nullptr){
            LOG(LogLevel::ERROR) << "first node is nullptr";
            return;
        }
        Node* node = firstNode;
        do{
            Intersector inter;
            inter.setSegment1(p1, points[node->getIndex()]);
            inter.setSegment2(p2, points[node->getIndex()]);
            RelativePosition relativePosition = inter.calculateRelativePosition();
            if (relativePosition == RelativePosition::Positive){
                insideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
                return;
            } else if (relativePosition == RelativePosition::Negative){
                outsideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
                return;
            }
            node = node->next;
        } while (node != firstNode);
        LOG(LogLevel::ERROR) << "relative position is parallel when no intersection";
    }
}

void Polygon::cutInsideOutsideConcave(std::vector<std::shared_ptr<std::vector<unsigned int>>>& insideIndices,
                                      std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsideIndices,
                                      const std::vector<Vector2f>& concaveVertices, const std::vector<unsigned int>& concaveIndices,
                                      RelativePosition relativePosition){
    orientation = RelativePosition::Positive;
    if (startNode && numberIntersections > 0){
        if (!startNode || startNode == nullptr){
            LOG(LogLevel::ERROR) << "Start node is nullptr and number of intersection is > 0";
            return;
        }
        startNode->touched = true;
        LOG(LogLevel::INFO) << startNode->getIndex() << " starting the cut";
        continueSmallPolygonInsideOutsideConcave(startNode, startNode, nullptr,
                                                 insideIndices, outsideIndices, concaveVertices, concaveIndices);
    } else{
        if (!firstNode || firstNode == nullptr){
            LOG(LogLevel::ERROR) << "first node is nullptr";
            return;
        }

        if (relativePosition == RelativePosition::Positive){
            insideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
        } else if (relativePosition == RelativePosition::Negative){
            outsideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
        } else{
            bool allBoundary = false;
            unsigned int correctIndex = 0;

            const std::vector<unsigned int>& indicesPoli = indices;
            unsigned int sizeIndicesPoli = indicesPoli.size();
            for (unsigned int i = 0; i < sizeIndicesPoli; i++){
                if (!isPointBoundaryConcavePolygon(points[indicesPoli[i]], concaveVertices, concaveIndices)){
                    correctIndex = i;
                    break;
                }
                if (i == sizeIndicesPoli - 1){
                    allBoundary = true;
                }
            }

            const Vector2f& point = points[indicesPoli[correctIndex]];
            if (allBoundary){
                LOG(LogLevel::INFO) << "Polygon::continueSmallPolygonInsideOutsideConcave: all points are boundary concave polygon";
                insideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
            } else if (isPointInsideConcavePolygon(point, concaveVertices, concaveIndices)){
                insideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
            } else{
                outsideIndices.push_back(std::make_shared<std::vector<unsigned int>>(indices));
            }
        }
    }
}

float Polygon::getWidth() const{
    return width;
}

float Polygon::getHeight() const{
    return height;
}

float Polygon::getXMin() const{
    return xMin;
}

float Polygon::getYMin() const{
    return yMin;
}

// STATIC

const double Polygon::BIG_DOUBLE = 1.0E+10;

double Polygon::CalculateArea(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices){
    double sum = 0;

    unsigned int sizeIndices = indices.size();
    for (unsigned int i = 0; i < sizeIndices; i++){
        sum += vertices[indices[i]].x                       * vertices[indices[(i + 1) % sizeIndices]].y -
               vertices[indices[(i + 1) % sizeIndices]].x   * vertices[indices[i]].y;
    }
    return abs(sum) / 2;
}

double Polygon::CalculateArea(const std::vector<Vector2f>& vertices){
    double sum = 0;

    unsigned int sizeVertices = vertices.size();
    for (unsigned int i = 0; i < sizeVertices; i++){
        sum += vertices[i].x                        * vertices[(i + 1) % sizeVertices].y -
               vertices[(i + 1) % sizeVertices].x   * vertices[i].y;
    }
    return abs(sum) / 2;
}

std::vector<Vector2f> Polygon::translateVertices(const std::vector<Vector2f>& vertices, float diff_x, float diff_y)
{
    unsigned int sizeVertices = vertices.size();
    std::vector<Vector2f> translated;

    for (unsigned int i = 0; i < sizeVertices; i++){
        translated.push_back(vertices[i] + Vector2f(diff_x, diff_y));
    }

    return translated;
}

std::shared_ptr<std::vector<Vector2f>> Polygon::translateVerticesPointer(const std::vector<Vector2f>& vertices, float diff_x, float diff_y)
{
    unsigned int sizeVertices = vertices.size();
    std::shared_ptr<std::vector<Vector2f>> translated = std::make_shared<std::vector<Vector2f>>();

    for (unsigned int i = 0; i < sizeVertices; i++){
        translated->push_back(vertices[i] + Vector2f(diff_x, diff_y));
    }

    return translated;
}

std::vector<Vector2f> Polygon::scaleVertices(const std::vector<Vector2f>& vertices,
                                             float originalWidth, float originalHeight, float newWidth, float newHeight)
{
    unsigned int sizeVertices = vertices.size();
    std::vector<Vector2f> scaled;

    for (unsigned int i = 0; i < sizeVertices; i++){
        scaled.push_back({((vertices[i].x + 1.0f) * newWidth / originalWidth) - 1.0f,
                          ((vertices[i].y + 1.0f) * newHeight / originalHeight) - 1.0f});
    }

    return scaled;
}

void Polygon::scaleXYMin(float* const originalXMin, float* const originalYMin,
                         float originalWidth, float originalHeight, float newWidth, float newHeight)
{
    if (originalXMin != nullptr){
        *originalXMin = ((*originalXMin + 1.0f) * newWidth / originalWidth) - 1.0f;
    }
    if (originalYMin != nullptr){
        *originalYMin = ((*originalYMin + 1.0f) * newHeight / originalHeight) - 1.0f;
    }
}

void Polygon::createBoundingBoxVariables(const std::vector<Vector2f>& vertices, float& width, float& height, float& xMin, float& yMin){
    unsigned int numberVertices = vertices.size();
    float right = vertices[0].x;
    float left = vertices[0].x;
    float top = vertices[0].y;
    float bottom = vertices[0].y;
    for (unsigned int i = 1; i < numberVertices; i++){
        const float& x = vertices[i].x;
        const float& y = vertices[i].y;
        if (x > right){
            right = x;
        } else if (x < left){
            left = x;
        }
        if (y > top){
            top = y;
        } else if (y < bottom){
            bottom = y;
        }
    }

    width = right - left;
    height = top - bottom;
    xMin = left;
    yMin = bottom;
}

bool Polygon::isPointBoundaryConcavePolygon(const Vector2f& point,
                                            const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices){
    Intersector inter;
    const unsigned int sizeIndices = indices.size();
    for (unsigned int i = 0; i < sizeIndices; i++){
        if (point == vertices[indices[i]] || point == vertices[indices[(i + 1) % sizeIndices]]){
            return true;
        }
        inter.setSegment1(vertices[indices[i]], vertices[indices[(i + 1) % sizeIndices]]);
        inter.setSegment2(vertices[indices[i]], point);
        if (inter.calculateRelativePosition() == RelativePosition::Parallel){
            const Vector2f diffVertices = vertices[indices[(i + 1) % sizeIndices]] - vertices[indices[i]];
            const Vector2f diffIntersection = point - vertices[indices[i]];
            if (diffVertices.dot(diffIntersection) > 0.0 &&
                Vector2f::IsFirstDoubleGreater(diffVertices.normSquared(), diffIntersection.normSquared())){
                return true;
            }
        }
    }
    return false;
}

bool Polygon::isPointInsideConcavePolygon(const Vector2f& point,
                                          const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices){
    unsigned int numberPositive = 0;
    unsigned int numberNegative = 0;

    const Vector2f& firstVector = {1, 1};

    if (point == firstVector){
        LOG(LogLevel::WARN) << "Polygon::isPointInsideConcavePolygon: point is on vertex";
        return true;
    }

    Intersector inter;

    inter.setSegment1(point, firstVector);

    const Vector2f reference = firstVector - point;

    unsigned int sizeIndices = indices.size();
    for (unsigned int i = 0; i < sizeIndices; i++){
        inter.setSegment2(vertices[indices[i]], vertices[indices[(i + 1) % sizeIndices]]);

        LineSegmentIntersection lineSegmentIntersection = inter.calculateLineSegmentIntersection();

//        if (lineSegmentIntersection == LineSegmentIntersection::Parallel){
//            LOG(LogLevel::WARN) << "line parallel";
//        }

        bool insideSegment = lineSegmentIntersection == LineSegmentIntersection::InsideSegment;
        bool secondVertex = lineSegmentIntersection == LineSegmentIntersection::SecondVertex;
        if (insideSegment || secondVertex){
            const Vector2f intersectionPoint = inter.getIntersectionPoint();
            const Vector2f difference = intersectionPoint - point;

            double dot = reference.dot(difference);

            // check that intersection point is not isolated
            if (secondVertex){
                inter.setSegment2(intersectionPoint, vertices[indices[i]]);
                RelativePosition beforeRelPos = inter.calculateRelativePosition();
                inter.setSegment2(intersectionPoint, vertices[indices[(i + 2) % sizeIndices]]);
                RelativePosition afterRelPos = inter.calculateRelativePosition();

                if (beforeRelPos == afterRelPos ||
                    beforeRelPos == RelativePosition::Parallel || afterRelPos == RelativePosition::Parallel){
                    continue;
                }
            }

            if (Vector2f::AreDoublesEqual(dot, 0.0)){
                LOG(LogLevel::ERROR) << "dot is 0";
            } else if (dot > 0){
                numberPositive++;
            } else if (dot < 0){
                numberNegative++;
            }
        }
    }

    bool positiveEven = (numberPositive % 2) == 0;
    bool negativeEven = (numberNegative % 2) == 0;

//    LOG(LogLevel::INFO) << "number positive: " << numberPositive << " number negative: " << numberNegative << std::endl;

    if (positiveEven && negativeEven){
        return false;
    } else if (!positiveEven && !negativeEven){
        return true;
    } else{
        LOG(LogLevel::WARN) << "odd intersections";
        return false;
    }
}

// PRIVATE

void Polygon::checkEnoughPointIndices() const{
    if (points.size() < 2){
        LOG(LogLevel::ERROR) << "number of points is less than 2";
    }

    if (indices.size() < 2){
        LOG(LogLevel::ERROR) << "number of indices is less than 2";
    }
}

bool Polygon::checkIsPointIntersection(const Node* node) const{
    const Node* next = node->next;
    const Node* previous = node->previous;
    if (!next || next == nullptr || !previous || previous == nullptr){
        LOG(LogLevel::WARN) << "node is not connected";
        return false;
    }
    Intersector inter;
    inter.setSegment1(p1, points[next->getIndex()]);
    inter.setSegment2(p2, points[next->getIndex()]);
    RelativePosition relNext = inter.calculateRelativePosition();

    inter.setSegment1(p1, points[previous->getIndex()]);
    inter.setSegment2(p2, points[previous->getIndex()]);
    RelativePosition relPreviuos = inter.calculateRelativePosition();

    if (relNext == RelativePosition::Parallel){
        LOG(LogLevel::WARN) << "relative position next is parallel when deciding point intersection";
        return false;
    }
    if (relPreviuos == RelativePosition::Parallel){
        LOG(LogLevel::WARN) << "relative position previous is parallel when deciding point intersection";
        return false;
    }
    if (relNext == relPreviuos){
        return false;
    }
    return true;
}

void Polygon::calculateOrientation(){
    if (!startNode->up || startNode->up == nullptr){
        LOG(LogLevel::WARN) << "start node does not have up, orientation set to positive";
        orientation = RelativePosition::Positive;
    } else{
        Intersector inter;
        inter.setSegment1(points[startNode->previous->getIndex()], points[startNode->up->getIndex()]);
        inter.setSegment2(points[startNode->next->getIndex()], points[startNode->up->getIndex()]);
        orientation = inter.calculateRelativePosition();
        if (orientation == RelativePosition::Parallel){
            //throw std::runtime_error("Error when calculating orientation");
            LOG(LogLevel::WARN) << "orientation is parallel, set to positive";
            orientation = RelativePosition::Positive;
        }
    }
    LOG(LogLevel::INFO) << "orientation: " << orientation;
}

const Node* Polygon::getNextIntersection(const Node *node){
    if (!node->isIntersection()){
        LOG(LogLevel::WARN) << node->getIndex() << " returned nullptr from Polygon::getNextIntersection";
        return nullptr;
    }
    if (node->up == nullptr){
        return node->down;
    }
    if (node->down == nullptr){
        return node->up;
    }
    Intersector inter;
    inter.setSegment1(points[node->previous->getIndex()], points[node->up->getIndex()]);
    inter.setSegment2(points[node->next->getIndex()], points[node->up->getIndex()]);
    if (inter.calculateRelativePosition() == orientation){
        LOG(LogLevel::INFO) << node->getIndex() << " same orientation";
        return node->up;
    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " opposite orientation";
        return node->down;
    }
}

void Polygon::sortIntersectionsNetwork(const std::vector<Node*>& nodes){
    unsigned int nodesLenght = nodes.size();
    if (nodesLenght == 0){
        return;
    }

    if (startNode == nullptr){
        LOG(LogLevel::ERROR) << "start node is nullptr";
        exit(-1);
    }
    startNode->down = startNode;
    Node* minNode = startNode;
    Node* node = startNode;

    Vector2f segment = p2 - p1;
    for (unsigned int i = 1; i < nodesLenght; i++){
        //LOG(LogLevel::DEBUG) << "node: " << minNode->getIndex();
        double minProduct = Polygon::BIG_DOUBLE;
        for (unsigned int n = 0; n < nodesLenght; n++){
            if (nodes[n]->down == nullptr){
                double product = segment.dot(points[nodes[n]->getIndex()] - p1);
                if (product < minProduct){
                    minProduct = product;
                    minNode = nodes[n];
                }
            }
        }
        minNode->down = node;
        node->up = minNode;
        node = minNode;
    }
    startNode->down = nullptr;
}

void Polygon::continueSmallPolygon(const Node* node, const Node* initialNode, std::vector<unsigned int>& indicesPoli,
                                   std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices){
    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;

    // we loop and add the nodes while we are not at an intersection node
    unsigned int previous = node->previous->getIndex();
    unsigned int now;
    while (!node->isIntersection() && node != initialNode) {
        now = node->getIndex();
        // we check that we don't add the same index two times
        // this happens very rarely, when there are a lot of vertices on the borders
        if (now == previous){
            LOG(LogLevel::WARN) << now << " same index as node before";
        } else{
            indicesPoli.push_back(node->getIndex());
        }
        previous = now;
        LOG(LogLevel::INFO) << "at " << node->getIndex();
        node = node->next;
    }

    LOG(LogLevel::INFO) << node->getIndex() << " arrived";
    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    if (node != initialNode){
        indicesPoli.push_back(node->getIndex());
    } else{
        // it happens very rarely when there are two intersection one after the other in really complex polygons
        LOG(LogLevel::WARN) << node->getIndex() << " closing polygon other way";
        indicesPoli.push_back(node->getIndex());
    }
    if (!node->touched){
        LOG(LogLevel::INFO) << node->getIndex() << " is not already touched";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        // here is where we will close the polygon
        if (node == initialNode){
            LOG(LogLevel::INFO) << node->getIndex() << " closing polygon";
        }
        // in the past: sometimes getNextIntersection returns a nullptr,
        // this happens very rarely when there are a lot of points and intersections
        // and intersector thinks that certain lines are parallel when in reality are not
        // now it is solved because we no longer loop until relative position change but until we reach an intersection node
        if (node != nullptr && node != initialNode){
            node->touched = true;
            LOG(LogLevel::INFO) << node->getIndex() << " continue samll polygon";
            // first we continue the small polygon we are creating
            continueSmallPolygon(node, initialNode, indicesPoli, polygonsIndices);
        }
        // here we create a new small polygon
        LOG(LogLevel::INFO) << nodeCreation->getIndex() << " create samll polygon";
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
        // we add a new array to the big array of arrays
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[polygonsIndices.size() - 1];
        continueSmallPolygon(nodeCreation, nodeCreation, indicesPoliCreation, polygonsIndices);
    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " is already touched";
    }
}

void Polygon::continueSmallPolygonInsideOutside(const Node* node, const Node* initialNode,
    std::shared_ptr<std::vector<unsigned int>> indicesPoliPoi,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& insidePolygonsIndices,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsidePolygonsIndices){

    // this check is used when the small polygon is just created and the relative position is parallel
    // becuse we are starting from an intersection point, if we not change it we will never enter in the while loop
    // we have to decide if the new small polygon is inside or outside

    if (indicesPoliPoi == nullptr){
        Intersector inter;
        Node* tryNode = node->next;
        RelativePosition relativePosition = RelativePosition::Parallel;
        while (relativePosition == RelativePosition::Parallel && tryNode != node){
            // here we compute the intersection with the two points in order to find if we are arrived at an intersection point
            Intersector inter;
            inter.setSegment1(p1, points[tryNode->getIndex()]);
            inter.setSegment2(p2, points[tryNode->getIndex()]);
            relativePosition = inter.calculateRelativePosition();
            tryNode = tryNode->next;
        }
        if (relativePosition == RelativePosition::Positive){
            insidePolygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
            indicesPoliPoi = insidePolygonsIndices[insidePolygonsIndices.size() - 1];
        } else if (relativePosition == RelativePosition::Negative){
            outsidePolygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
            indicesPoliPoi = outsidePolygonsIndices[outsidePolygonsIndices.size() - 1];
        } else if (relativePosition == RelativePosition::Parallel){
//            relativePosition = RelativePosition::Positive;
            LOG(LogLevel::ERROR) << "relative position is parallel every time";
            return;
        }
    }

    if (!indicesPoliPoi || indicesPoliPoi == nullptr){
        LOG(LogLevel::ERROR) << "indices poli pointer is nullptr";
        return;
    }
    std::vector<unsigned int>& indicesPoli = *indicesPoliPoi;

    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;

    // we loop and add the nodes while we are not at an intersection node
    unsigned int previous = node->previous->getIndex();
    unsigned int now;
    while (!node->isIntersection() && node != initialNode) {
        now = node->getIndex();
        // we check that we don't add the same index two times
        // this happens very rarely, when there are a lot of vertices on the borders
        if (now == previous){
            LOG(LogLevel::WARN) << now << " same index as node before";
        } else{
            indicesPoli.push_back(node->getIndex());
        }
        previous = now;
        LOG(LogLevel::INFO) << "at " << node->getIndex();
        node = node->next;
    }

    LOG(LogLevel::INFO) << node->getIndex() << " arrived";

    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    indicesPoli.push_back(node->getIndex());

    if (node == initialNode){
        // it happens very rarely when there are two intersection one after the other in really complex polygons
        LOG(LogLevel::WARN) << node->getIndex() << " closing polygon other way";
    }
    if (!node->touched){
        LOG(LogLevel::INFO) << node->getIndex() << " is not already touched";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        // here is where we will close the polygon
        if (node == initialNode){
            LOG(LogLevel::INFO) << node->getIndex() << " closing polygon";
        }
        // in the past: sometimes getNextIntersection returns a nullptr,
        // this happens very rarely when there are a lot of points and intersections
        // and intersector thinks that certain lines are parallel when in reality are not
        // now it is solved because we no longer loop until relative position change but until we reach an intersection node
        if (node != nullptr && node != initialNode){
            node->touched = true;
            LOG(LogLevel::INFO) << node->getIndex() << " continue samll polygon";
            // first we continue the small polygon we are creating

            continueSmallPolygonInsideOutside(node, initialNode, indicesPoliPoi,
                                              insidePolygonsIndices, outsidePolygonsIndices);
        }
        // here we create a new small polygon
        LOG(LogLevel::INFO) << nodeCreation->getIndex() << " create samll polygon";

        // we add a new array to the big array of arrays
        continueSmallPolygonInsideOutside(nodeCreation, nodeCreation, nullptr,
                                          insidePolygonsIndices, outsidePolygonsIndices);
    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " is already touched";
    }
}

void Polygon::continueSmallPolygonInsideOutsideConcave(const Node* node, const Node* initialNode,
    std::shared_ptr<std::vector<unsigned int>> indicesPoliPoi,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& insidePolygonsIndices,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsidePolygonsIndices,
    const std::vector<Vector2f>& concaveVertices, const std::vector<unsigned int>& concaveIndices){

    bool closing = false;

    if (indicesPoliPoi == nullptr){
        indicesPoliPoi = std::make_shared<std::vector<unsigned int>>();
    }

    if (!indicesPoliPoi || indicesPoliPoi == nullptr){
        LOG(LogLevel::ERROR) << "indices poli pointer is nullptr";
        return;
    }
    std::vector<unsigned int>& indicesPoli = *indicesPoliPoi;

    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;

    // we loop and add the nodes while we are not at an intersection node
    unsigned int previous = node->previous->getIndex();
    unsigned int now;
    while (!node->isIntersection() && node != initialNode) {
        now = node->getIndex();
        // we check that we don't add the same index two times
        // this happens very rarely, when there are a lot of vertices on the borders
        if (now == previous){
            LOG(LogLevel::WARN) << now << " same index as node before";
        } else{
            indicesPoli.push_back(node->getIndex());
        }
        previous = now;
        LOG(LogLevel::INFO) << "at " << node->getIndex();
        node = node->next;
    }

    LOG(LogLevel::INFO) << node->getIndex() << " arrived";

    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    indicesPoli.push_back(node->getIndex());

    if (node == initialNode){
        // it happens very rarely when there are two intersection one after the other in really complex polygons
        LOG(LogLevel::WARN) << node->getIndex() << " closing polygon other way";
    }
    if (!node->touched){
        LOG(LogLevel::INFO) << node->getIndex() << " is not already touched";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        if (node == nullptr){
            LOG(LogLevel::ERROR) << "getNextIntersection returned nullptr";
            return;
        }
        // here is where we will close the polygon
        if (node == initialNode){
            LOG(LogLevel::INFO) << node->getIndex() << " closing polygon";
        }
        // in the past: sometimes getNextIntersection returns a nullptr,
        // this happens very rarely when there are a lot of points and intersections
        // and intersector thinks that certain lines are parallel when in reality are not
        // now it is solved because we no longer loop until relative position change but until we reach an intersection node
        if (node != initialNode){
            node->touched = true;
            LOG(LogLevel::INFO) << node->getIndex() << " continue samll polygon";
            // first we continue the small polygon we are creating

            continueSmallPolygonInsideOutsideConcave(node, initialNode, indicesPoliPoi,
                                              insidePolygonsIndices, outsidePolygonsIndices, concaveVertices, concaveIndices);
        } else{
            closing = true;
        }
        // here we create a new small polygon
        LOG(LogLevel::INFO) << nodeCreation->getIndex() << " create samll polygon";

        // we add a new array to the big array of arrays
        continueSmallPolygonInsideOutsideConcave(nodeCreation, nodeCreation, nullptr,
                                          insidePolygonsIndices, outsidePolygonsIndices, concaveVertices, concaveIndices);
    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " is already touched";
        closing = true;
    }
    if (closing){
        // we decide if the polygon is inside or outside
        bool allBoundary = false;
        unsigned int correctIndex = 0;

        unsigned int sizeIndicesPoli = indicesPoli.size();
        for (unsigned int i = 0; i < sizeIndicesPoli; i++){
            if (!isPointBoundaryConcavePolygon(points[indicesPoli[i]], concaveVertices, concaveIndices)){
                correctIndex = i;
                break;
            }
            if (i == sizeIndicesPoli - 1){
                allBoundary = true;
            }
        }

        const Vector2f& point = points[indicesPoli[correctIndex]];
        if (allBoundary){
            LOG(LogLevel::INFO) << "Polygon::continueSmallPolygonInsideOutsideConcave: all points are boundary concave polygon";
            insidePolygonsIndices.push_back(indicesPoliPoi);
        } else if (isPointInsideConcavePolygon(point, concaveVertices, concaveIndices)){
            insidePolygonsIndices.push_back(indicesPoliPoi);
        } else{
            outsidePolygonsIndices.push_back(indicesPoliPoi);
        }
    }
}
