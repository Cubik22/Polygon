#include "Polygon.h"
#include "Logger.h"
#include <iostream>
#include <exception>

const double Polygon::BIG_DOUBLE = 1.0E+10;

Polygon::Polygon(){}

Polygon::Polygon(const std::vector<Vector2f> &_points, const std::vector<unsigned int> &_indices) :
    points{_points}, indices{_indices}{
    if (_points.size() != _indices.size()){
        throw std::runtime_error("Points vector and indices list should have the same size");
    }
}

Polygon::~Polygon(){
    LOG(LogLevel::DEBUG) << "polygon deleted";
    if (startNode){
        Network::deleteNetwork(startNode);
    }
}

void Polygon::setBody(const std::vector<Vector2f> &_points, const std::vector<unsigned int> &_indices){
    if (_points.size() != _indices.size()){
        throw std::runtime_error("Points vector and indices list should have the same size");
    }
    points = _points;
    indices = _indices;
}

void Polygon::setSegment(const Vector2f &_p1, const Vector2f &_p2){
    p1 = _p1;
    p2 = _p2;
}

const Vector2f& Polygon::getPoint(unsigned int index) const{
    if (index >= getNumberIndices()){
        throw std::runtime_error("Array out of bound");
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

void Polygon::printVertices() const{
    LOG(LogLevel::INFO) << "Printing polygon vertices";
    for (unsigned int i = 0; i < points.size(); i++){
        LOG(LogLevel::INFO) << i << " x: " << points[i].x << " y: " << points[i].y;
    }
}

void Polygon::printIndices() const{
    LOG(LogLevel::INFO) << "Printing polygon indices";
    for (unsigned int i = 0; i < indices.size(); i++){
        LOG(LogLevel::INFO) << i << " " << indices[i];
    }
}

void Polygon::createNetwork(){
    // stores the intersection Nodes in order to sort them after
    std::vector<Node*> unorderedIntersectionNodes;
    // intersector is used to find intersection points
    Intersector inter;
    // this is the segment, it will be treated as a line when computing intersection
    inter.setSegment1(p1, p2);
    unsigned int numberIndices = getNumberIndices();
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
    Node* firstNode = nullptr;
    // this stores the pointer to the temporary node we are considering
    Node* node = nullptr;
    // this stores the pointer to the previous node se we can connect it with the new one
    Node* previous = nullptr;

    for (unsigned int i = 0; i < numberIndices; i++){
        node = new Node(indices[i]);
        // previous will be nullptr just the first time
        if (previous != nullptr){
            node->previous = previous;
            previous->next = node;
        } else{
            //std::cout << "first node setted\n";
            firstNode = node;
        }
        // here we set previous to node beacuse in the next iteration will be previous
        previous = node;
        // we calculate the intersection with the segment from the point to the next one
        inter.setSegment2(points[i], points[(i + 1) % numberIndices]);
        IntersectionType intersectionType = inter.calculateIntersection(true, false);
        if (intersectionType == IntersectionType::InsideSegment){
            const Vector2f intersectionPoint = inter.getIntersectionPoint();
            // we add the new point found to the points of the polygon
            points.push_back(intersectionPoint);
            // index of last point before intersection was numberIndices - 1 so the first intersection node starts from numberIndices
            node = new Node(numberIndices + numberIntersections);
            // we add the new node found to the list of unordered intersection nodes
            unorderedIntersectionNodes.push_back(node);
            numberIntersections++;
            // again we connect it with the previous node because intersection nodes are connected both like the normal nodes to previous and after
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
    //std::cout << "end loop\n";
    if (firstNode == nullptr){
        LOG(LogLevel::ERROR) << "first node is nullptr";
        exit(-1);
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

std::vector<std::vector<unsigned int>*> Polygon::cut(){
    std::vector<std::vector<unsigned int>*> polygonsIndices;
    if (numberIntersections > 0){
        startNode->touched = true;
        LOG(LogLevel::INFO) << startNode->getIndex() << " starting the cut";
        polygonsIndices.push_back(new std::vector<unsigned int>);
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[0];
        continueSmallPolygon(startNode, startNode, RelativePosition::Parallel, indicesPoliCreation, polygonsIndices);
    } else{
        polygonsIndices.push_back(new std::vector<unsigned int>(indices));
    }
    return  polygonsIndices;
}

// PRIVATE

void Polygon::calculateOrientation(){
    Intersector inter;
    inter.setSegment1(points[startNode->previous->getIndex()], points[startNode->up->getIndex()]);
    inter.setSegment2(points[startNode->next->getIndex()], points[startNode->up->getIndex()]);
    RelativePosition relativePosition = inter.calculateRelativePosition();
    if (relativePosition == RelativePosition::Parallel){
        throw std::runtime_error("Error when calculating orientation");
    }
    LOG(LogLevel::INFO) << "orientation: " << relativePosition;
    orientation = relativePosition;
}

const Node* Polygon::getNextIntersection(const Node *node){
    if (node->up == nullptr && node->down == nullptr){
        LOG(LogLevel::WARN) << node->getIndex() << "returned nullptr from Polygon::getNextIntersection";
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

void Polygon::continueSmallPolygon(const Node* node, const Node* initialNode, RelativePosition relativePosition,
                                   std::vector<unsigned int>& indicesPoli, std::vector<std::vector<unsigned int>*>& polygonsIndices){
    // first we add the node we currently are at to the list of indices of the small polygon
    indicesPoli.push_back(node->getIndex());
    node = node->next;
    // here we compute the intersection with the two points in order to find if we are arrived at an intersection point
    Intersector inter;
    inter.setSegment1(p1, points[node->getIndex()]);
    inter.setSegment2(p2, points[node->getIndex()]);
    // this check is used when the small polygon is just created and the relative position is parallel
    // becuse we are starting from an intersection point, if we not change it we will never enter in the while loop
    if (relativePosition == RelativePosition::Parallel){
        relativePosition = inter.calculateRelativePosition();
    }
    // we loop until we arrive at an intersection point, so when the relative position becomes parallel
    while (inter.calculateRelativePosition() == relativePosition) {
        indicesPoli.push_back(node->getIndex());
        node = node->next;
        inter.setSegment1(p1, points[node->getIndex()]);
        inter.setSegment2(p2, points[node->getIndex()]);
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
        // sometimes getNextIntersection returns a nullptr, this happens very rarely when there are a lot of points and intersections
        // and intersector thinks that certain lines are parallel when in reality are not
        if (node != nullptr && node != initialNode){
            node->touched = true;
            LOG(LogLevel::INFO) << node->getIndex() << " continue samll polygon";
            // first we continue the small polygon we are creating
            continueSmallPolygon(node, initialNode, relativePosition, indicesPoli, polygonsIndices);
        }
        // here we create a new small polygon
        LOG(LogLevel::INFO) << nodeCreation->getIndex() << " create samll polygon";
        polygonsIndices.push_back(new std::vector<unsigned int>);
        // we add a new array to the big array of arrays
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[polygonsIndices.size() - 1];
        continueSmallPolygon(nodeCreation, nodeCreation, RelativePosition::Parallel, indicesPoliCreation, polygonsIndices);
    } else{
        LOG(LogLevel::INFO) << node->getIndex() << " is already touched";
    }
}

