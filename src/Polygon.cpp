#include "Polygon.h"
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
    //std::cout <<"polygon deleted\n";
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

const Node* Polygon::getStartNode() const{
    return startNode;
}

unsigned int Polygon::getNumberIntersections() const{
    return numberIntersections;
}

void Polygon::printVertices() const{
    std::cout << "Printing polygon vertices\n";
    for (unsigned int i = 0; i < points.size(); i++){
        std::cout << i << " x: " << points[i].x << " y: " << points[i].y << "\n";
    }
}

void Polygon::printIndices() const{
    std::cout << "Printing polygon indices\n";
    for (unsigned int i = 0; i < indices.size(); i++){
        std::cout << i << " " << indices[i] << "\n";
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
        std::cerr << "ERROR: first node is nullptr\n";
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
        std::cout << std::endl;

    } else if (numberIntersections == 0){
        std::cout << "No intersections\n";
        startNode = firstNode;
    } else{
        std::cerr << "ERROR: problems when setting startNode\n";
        startNode = firstNode;
    }
}

std::vector<std::vector<unsigned int>*> Polygon::cut(){
    std::vector<std::vector<unsigned int>*> polygonsIndices;
    if (numberIntersections > 0){
        startNode->touched = true;
        std::cout << startNode->getIndex() << " starting the cut\n";
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
    std::cout << "orientation: " << relativePosition << "\n";
    orientation = relativePosition;
}

const Node* Polygon::getNextIntersection(const Node *node){
    if (node->up == nullptr && node->down == nullptr){
        std::cout << node->getIndex() << "ERROR: returned nullptr from Polygon::getNextIntersection\n";
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
        std::cout << node->getIndex() << " same orientation\n";
        return node->up;
    } else{
        std::cout << node->getIndex() << " opposite orientation\n";
        return node->down;
    }
}

void Polygon::sortIntersectionsNetwork(const std::vector<Node*>& nodes){
    unsigned int nodesLenght = nodes.size();
    if (nodesLenght == 0){
        return;
    }

    if (startNode == nullptr){
        std::cerr << "ERROR: start node is nullptr\n";
        exit(-1);
    }
    startNode->down = startNode;
    Node* minNode = startNode;
    Node* node = startNode;

    Vector2f segment = p2 - p1;
    for (unsigned int i = 1; i < nodesLenght; i++){
        //std::cout << "node: " << minNode->getIndex() << "\n";
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
    std::cout << node->getIndex() << " arrived\n";
    // if the node we are arrived at is the starting node, we are finished, the small polygon is closed
    if (node != initialNode){
        indicesPoli.push_back(node->getIndex());
    } else{
        // theoretically it will never happen that we close the small polygon here, but I'm not sure :)
        std::cout << node->getIndex() << " closing polygon other way\n";
    }
    if (!node->touched){
        std::cout << node->getIndex() << " is not already touched\n";
        node->touched = true;
        const Node* nodeCreation = node;
        node = getNextIntersection(node);
        // theoretically here is where we will close the polygon
        if (node == initialNode){
            std::cout << node->getIndex() << " closing polygon\n";
        }
        // sometimes getNextIntersection returns a nullptr, this happens very rarely when there are a lot of points and intersections
        // and intersector thinks that certain lines are parallel when in reality are not
        if (node != nullptr && node != initialNode){
            node->touched = true;
            std::cout << node->getIndex() << " continue samll polygon\n";
            // first we continue the small polygon we are creating
            continueSmallPolygon(node, initialNode, relativePosition, indicesPoli, polygonsIndices);
        }
        // here we create a new small polygon
        std::cout << nodeCreation->getIndex() << " create samll polygon\n";
        polygonsIndices.push_back(new std::vector<unsigned int>);
        // we add a new array to the big array of arrays
        std::vector<unsigned int>& indicesPoliCreation = *polygonsIndices[polygonsIndices.size() - 1];
        continueSmallPolygon(nodeCreation, nodeCreation, RelativePosition::Parallel, indicesPoliCreation, polygonsIndices);
    } else{
        std::cout << node->getIndex() << " is already touched\n";
    }
}

