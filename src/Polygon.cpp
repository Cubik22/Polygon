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

        if (intersectionType == IntersectionType::InsideSegment){
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
        }
    }

    if (firstNode == nullptr){
        LOG(LogLevel::ERROR) << "first node is nullptr";
        return;
    }
    // finally we connect the first node with the last one and the cycle is closed
    firstNode->previous = previous;
    previous->next = firstNode;

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
        // (if it is inside the segment but on the vertex of the segment intended as line intersection point is FirstOnVertex)
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
        continueSmallPolygonInsideOutside(startNode, startNode, RelativePosition::Parallel, nullptr, insideIndices, outsideIndices);
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

// TODO: area has to be calculated from indices not vertices because there can be more vertices than indices
float Polygon::calculateArea() const
{
    float sum =0;

    bool first = true;
    Vector2f previous;
    for (const Vector2f& v : points){

        if (first == true)
            sum = sum + points[points.size()].x * v.y - v.x*points[points.size()].y;
        else
            sum = sum + previous.x* v.y - v.x * previous.y;

        first = false;
        previous = v;
    }
    return abs(sum) / 2;
}

// STATIC

const double Polygon::BIG_DOUBLE = 1.0E+10;

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

// PRIVATE

void Polygon::checkEnoughPointIndices() const{
    if (points.size() < 2){
        LOG(LogLevel::ERROR) << "number of points is less than 2";
    }

    if (indices.size() < 2){
        LOG(LogLevel::ERROR) << "number of indices is less than 2";
    }
}

// TODO: when is parallel try with next two
void Polygon::calculateOrientation(){
    Intersector inter;
    inter.setSegment1(points[startNode->previous->getIndex()], points[startNode->up->getIndex()]);
    inter.setSegment2(points[startNode->next->getIndex()], points[startNode->up->getIndex()]);
    RelativePosition relativePosition = inter.calculateRelativePosition();
    if (relativePosition == RelativePosition::Parallel){
        //throw std::runtime_error("Error when calculating orientation");
        LOG(LogLevel::WARN) << "orientation is parallel, set to positive";
        relativePosition = RelativePosition::Positive;
    }
    LOG(LogLevel::INFO) << "orientation: " << relativePosition;
    orientation = relativePosition;
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
//    const Node* startNode = node;
    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;

    // we loop and add the nodes while we are not at an intersection node
    while (!node->isIntersection() && node != initialNode) {
        indicesPoli.push_back(node->getIndex());
        node = node->next;
    }

    LOG(LogLevel::INFO) << node->getIndex() << " arrived";
    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    if (node != initialNode){
        indicesPoli.push_back(node->getIndex());
    } else{
        // theoretically it will never happen that we close the small polygon here, but I'm not sure :)
        LOG(LogLevel::WARN) << node->getIndex() << " closing polygon other way";
    }
    if (!node->touched){
        LOG(LogLevel::INFO) << node->getIndex() << " is not already touched";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        // theoretically here is where we will close the polygon
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
    RelativePosition relativePosition, std::shared_ptr<std::vector<unsigned int>> indicesPoliPoi,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& insidePolygonsIndices,
    std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsidePolygonsIndices){

//    const Node* startNode = node;
    // this check is used when the small polygon is just created and the relative position is parallel
    // becuse we are starting from an intersection point, if we not change it we will never enter in the while loop

    Intersector inter;

    Node* tryNode = node->next;
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

    if (!indicesPoliPoi || indicesPoliPoi == nullptr){
        LOG(LogLevel::ERROR) << "indices poli pointer is nullptr";
        return;
    }
    std::vector<unsigned int>& indicesPoli = *indicesPoliPoi;

    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;

    inter.setSegment1(p1, points[node->getIndex()]);
    inter.setSegment2(p2, points[node->getIndex()]);

    // we loop until we arrive at an intersection point, so when the relative position becomes parallel
    while (!node->isIntersection() && node != initialNode) {
        indicesPoli.push_back(node->getIndex());
        node = node->next;
    }

    LOG(LogLevel::INFO) << node->getIndex() << " arrived";
    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    if (node != initialNode){
        indicesPoli.push_back(node->getIndex());
    } else{
        // theoretically it will never happen that we close the small polygon here, but I'm not sure :)
        LOG(LogLevel::WARN) << node->getIndex() << " closing polygon other way";
    }
    if (!node->touched){
        LOG(LogLevel::INFO) << node->getIndex() << " is not already touched";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        // theoretically here is where we will close the polygon
        if (node == initialNode){
            LOG(LogLevel::INFO) << node->getIndex() << " closing polygon";
        }

        if (node != nullptr && node != initialNode){
            node->touched = true;
            LOG(LogLevel::INFO) << node->getIndex() << " continue samll polygon";
            // first we continue the small polygon we are creating

            continueSmallPolygonInsideOutside(node, initialNode, relativePosition, indicesPoliPoi,
                                              insidePolygonsIndices, outsidePolygonsIndices);
        }
        // here we create a new small polygon
        LOG(LogLevel::INFO) << nodeCreation->getIndex() << " create samll polygon";
        // we add a new array to the big array of arrays
        if (relativePosition == RelativePosition::Positive){
//            insidePolygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
//            std::shared_ptr<std::vector<unsigned int>>& indicesPoliCreation = insidePolygonsIndices[insidePolygonsIndices.size() - 1];
//            continueSmallPolygonInsideOutside(nodeCreation, nodeCreation, RelativePosition::Negative, indicesPoliCreation,
//                                              insidePolygonsIndices, outsidePolygonsIndices);
            continueSmallPolygonInsideOutside(nodeCreation, nodeCreation, RelativePosition::Parallel, nullptr,
                                              insidePolygonsIndices, outsidePolygonsIndices);
        } else if (relativePosition == RelativePosition::Negative){
//            outsidePolygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
//            std::shared_ptr<std::vector<unsigned int>>& indicesPoliCreation = outsidePolygonsIndices[outsidePolygonsIndices.size() - 1];
//            continueSmallPolygonInsideOutside(nodeCreation, nodeCreation, RelativePosition::Positive, indicesPoliCreation,
//                                              insidePolygonsIndices, outsidePolygonsIndices);
            continueSmallPolygonInsideOutside(nodeCreation, nodeCreation, RelativePosition::Parallel, nullptr,
                                              insidePolygonsIndices, outsidePolygonsIndices);
        }

    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " is already touched";
    }
}
