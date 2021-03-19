#include "Polygon.h"
#include <iostream>
#include <exception>

Polygon::Polygon(){}

Polygon::~Polygon(){
    std::cout <<"polygon deleted\n";
    delete startNode;
}

Polygon::Polygon(const std::vector<Vector2f> &_points, const std::vector<unsigned int> &_indices) :
    points{_points}, indices{_indices}{
    if (_points.size() != _indices.size()){
        throw std::runtime_error("Points vector and indices list should have the same size");
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

const Vector2f &Polygon::getPoint(unsigned int index) const{
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

//void Polygon::addPointAfter(const Vector2f &point, unsigned int previous){
//    std::vector<unsigned int>& indices = body.indices;
//    body.points.push_back(point);
//    bool find = false;
//    for (std::list<unsigned int>::iterator it = indices.begin(); it != indices.end(); it++){
//        if (find == true){
//            indices.insert(it, getNumberIndices());
//            numberIndices++;
//            return;
//        }
//        if (*it == previous){
//            find = true;
//        }
//    }
//    indices.push_back(getNumberIndices());
//    numberIndices++;
//}

//void Polygon::addPointAt(const Vector2f &point, std::list<unsigned int>::const_iterator& it){
//    std::list<unsigned int>& indices = body.indices;
//    body.points.push_back(point);
//    //it++;
//    if (it != indices.end()){
//        startIndices = indices.insert(it, numberIndices);
//    } else{
//        //chiamata
//        indices.push_back(numberIndices);
//    }
//    numberIndices++;
//}

void Polygon::createNetwork(){
    Intersector inter;
    inter.setSegment1(p1, p2);
    unsigned int numberIndices = getNumberIndices();
    unsigned int numberIntersections = 0;
    double maxDistance = 0.0;
    Node* wrongEdge = nullptr;
    Node* firstNode = nullptr;
    Node* firstIntersection = nullptr;
    Node* lastIntersection = nullptr;
    Node* node = nullptr;
    Node* previous = nullptr;
    Node* down = nullptr;
    for (unsigned int i = 0; i < numberIndices; i++){
        node = new Node(indices[i]);
        if (previous != nullptr){
            node->previous = previous;
            previous->next = node;
        } else{
            //std::cout << "first node setted\n";
            firstNode = node;
        }
        previous = node;
        inter.setSegment2(points[i], points[(i + 1) % numberIndices]);
        IntersectionType intersectionType = inter.calculateIntersection(true, false);
        if (intersectionType == IntersectionType::InsideSegment){
            const Vector2f intersectionPoint = inter.getIntersectionPoint();
            points.push_back(intersectionPoint);
            node = new Node(numberIndices + numberIntersections);
            numberIntersections++;
            node->previous = previous;
            previous->next = node;
            previous = node;
            if (down != nullptr){
                node->down = down;
                down->up = node;
                double distanceUp = (points[down->up->getIndex()] - points[down->getIndex()]).normSquared();
                if (distanceUp > maxDistance){
                    maxDistance = distanceUp;
                    wrongEdge = down;
                }
                lastIntersection = node;
            } else{
                firstIntersection = node;
            }
            down = node;
        }
    }
    //std::cout << "fine ciclo\n";
    firstNode->previous = previous;
    previous->next = firstNode;
    firstIntersection->down = down;
    down->up = firstIntersection;
    double distanceUp = (points[down->up->getIndex()] - points[down->getIndex()]).normSquared();
    if (distanceUp > maxDistance){
        maxDistance = distanceUp;
        wrongEdge = down;
    }
    startNode = wrongEdge;
    calculateOrientation();
}

void Polygon::calculateOrientation(){
    Intersector inter;
    inter.setSegment1(points[startNode->previous->getIndex()], points[startNode->down->getIndex()]);
    inter.setSegment2(points[startNode->next->getIndex()], points[startNode->down->getIndex()]);
    RelativePosition relativePosition = inter.calculateRelativePosition();
    if (relativePosition == RelativePosition::Parallel){
        throw std::runtime_error("Error when calculating orientation");
    }
    std::cout << relativePosition;
    orientation = relativePosition;
}

bool Polygon::isNodeIntersectionDouble(const Node *node){
    if (node->up == nullptr){
        return false;
    }
    Intersector inter;
    inter.setSegment1(points[node->previous->getIndex()], points[node->up->getIndex()]);
    inter.setSegment2(points[node->next->getIndex()], points[node->up->getIndex()]);
    if (inter.calculateRelativePosition() == orientation){
        std::cout << node->getIndex() << " orientation uguale\n";
        return true;
    } else{
        std::cout << node->getIndex() << " orientation opposta\n";
        return false;
    }
}

void Polygon::sortIntersectionsNetwork(const std::vector<Node*>& nodes){
    unsigned int nodesLenght = nodes.size();
    Vector2f segment = p2 - p1;
    double minProduct = segment.dot(points[nodes[0]->getIndex()]);
    double product;
    Node* minNode = nodes[0];
    for (unsigned int i = 1; i < nodesLenght; i++){
        product = segment.dot(points[nodes[i]->getIndex()] - p1);
        if (product < minProduct){
            minProduct = product;
            minNode = nodes[i];
        }
    }
    minNode->down = minNode;
    startNode = minNode;
    Node* node = minNode;
    for (unsigned int i = 1; i < nodesLenght; i++){
        minProduct = 1.0E+10;
        for (unsigned int n = 0; n < nodesLenght; n++){
            if (nodes[n]->down != nullptr){
                product = segment.dot(points[nodes[n]->getIndex()] - p1);
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

//void Polygon::pushAndGo(std::vector<Vector2f> &positionsPoli, std::list<unsigned int> &indicesPoli, std::list<unsigned int>::const_iterator& it){
//    positionsPoli.push_back(body.points[*it]);
//    indicesPoli.push_back(*it);
//    //std::cout << "Prima: " << *it;
//    it++;
//    //std::cout << "   Dopo: " << *it << "      End: " << *(body.indices.end()) << "\n";
//    if (it == body.indices.end()){
//        it = body.indices.begin();
//    }
//}

void Polygon::createSmallPolygon(const Node* node, std::vector<std::vector<unsigned int>*>& polygonsIndices){
    std::cout << node->getIndex() << " create samll polygon\n";
    const Node* initialNode = node;
    polygonsIndices.push_back(new std::vector<unsigned int>);
    std::vector<unsigned int>& indicesPoli = *polygonsIndices[polygonsIndices.size() - 1];
    indicesPoli.push_back(node->getIndex());
    node = node->next;
    Intersector inter;
    inter.setSegment1(p1, points[node->getIndex()]);
    inter.setSegment2(p2, points[node->getIndex()]);
    RelativePosition relativePosition = inter.calculateRelativePosition();
    while (inter.calculateRelativePosition() == relativePosition) {
        indicesPoli.push_back(node->getIndex());
        node = node->next;
        inter.setSegment1(p1, points[node->getIndex()]);
        inter.setSegment2(p2, points[node->getIndex()]);
    }
    if (initialNode == startNode){
        createSmallPolygon(node, polygonsIndices);
        std::cout << "qui\n";
        indicesPoli.push_back(node->getIndex());
        std::cout << "sotto\n";
        continueSmallPolygon(node->up, initialNode, relativePosition, indicesPoli, polygonsIndices);
    } else{
        indicesPoli.push_back(node->getIndex());
    }
}

void Polygon::continueSmallPolygon(const Node* node, const Node* initialNode, RelativePosition relativePosition,
                                   std::vector<unsigned int>& indicesPoli, std::vector<std::vector<unsigned int>*>& polygonsIndices){
    std::cout << node->getIndex() << " continue samll polygon\n";
    indicesPoli.push_back(node->getIndex());
    node = node->next;
    Intersector inter;
    inter.setSegment1(p1, points[node->getIndex()]);
    inter.setSegment2(p2, points[node->getIndex()]);
    while (inter.calculateRelativePosition() == relativePosition) {
        indicesPoli.push_back(node->getIndex());
        node = node->next;
        inter.setSegment1(p1, points[node->getIndex()]);
        inter.setSegment2(p2, points[node->getIndex()]);
    }
    if (node == initialNode){
        return;
    }
    bool isNodeDouble = isNodeIntersectionDouble(node);
    if (isNodeDouble){
        createSmallPolygon(node, polygonsIndices);
        indicesPoli.push_back(node->getIndex());
        node = node->up;
        if (node == initialNode){
            return;
        }
        continueSmallPolygon(node, initialNode, relativePosition, indicesPoli, polygonsIndices);
    }
}

std::vector<std::vector<unsigned int>*> Polygon::cut(){
    std::vector<std::vector<unsigned int>*> polygonsIndices;
    createSmallPolygon(startNode, polygonsIndices);
//    Node* node = startNode;
//    Intersector inter;
//    inter.setSegment1(p1, points[node->getIndex()]);
//    inter.setSegment2(p2, points[node->getIndex()]);
//    RelativePosition posPolygon = inter.calculateRelativePosition();
//    int epoch = 0;
//    do {
//        std::vector<unsigned int> indicesPoli;
//        indicesPoli.push_back(node->getIndex());
//        node = node->next;
//        inter.setSegment1(p1, points[node->getIndex()]);
//        inter.setSegment2(p2, points[node->getIndex()]);
//        RelativePosition posTemp = inter.calculateRelativePosition();
//        //std::cout << "posPolygon: " << posPolygon << "     posTemp: " << posTemp << "\n";
//        if (posPolygon == RelativePosition::Parallel){
//            posPolygon = posTemp;
//        }
//        //int tempo = 0;
//        while (posTemp == posPolygon){
//            //std::cout << *it <<"\n";
//            indicesPoli.push_back(node->getIndex());
//            node = node->next;
//            inter.setSegment1(p1, points[node->getIndex()]);
//            inter.setSegment2(p2, points[node->getIndex()]);
//            posTemp = inter.calculateRelativePosition();
//        }
//        indicesPoli.push_back(node->getIndex());
//        polygonsIndices.push_back(indicesPoli);
//        posPolygon = posTemp;
//        //std::cout << "*it: " << *it << "        *startIndices: " << *startIndices << "\n";
//    } while (node != startNode);
    std::cout << "size 0: " << polygonsIndices[0]->size() << "\n";
    return  polygonsIndices;
}

const Node* Polygon::getStartNode() const{
    return startNode;
}

void Polygon::printVertices() const{
    for (unsigned int i = 0; i < points.size(); i++){
        std::cout << i << " x: " << points[i].x << " y: " << points[i].y << "\n";
    }
}

void Polygon::printIndices() const{
    for (unsigned int i = 0; i < indices.size(); i++){
        std::cout << i << " " << indices[i] << "\n";
    }
}
