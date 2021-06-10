#include "Element.h"
#include "Logger.h"
#include <exception>
#include <math.h>

#define FORWARD     true
#define BACKWARD    false
#define BIGGER      true
#define SMALLER     false
#define X_CONSIDER  true
#define Y_CONSIDER  false

Element::Element(const Polygon& _poly) :
    points(_poly.getPoints()), vertices(_poly.getPoints()), indices(_poly.getIndices()),
    numberStartIndices{_poly.getNumberIndices()}, numberAddedVertices{0}, created{false} {}

Element::Element(const std::vector<Vector2f>& _points, const std::vector<unsigned int>& _indices) :
    points(_points), vertices(_points), indices(_indices),
    numberStartIndices{(unsigned int)_indices.size()}, numberAddedVertices{0}, created{false} {}

const std::vector<std::shared_ptr<std::vector<unsigned int>>>& Element::createElement(){
//    LOG(LogLevel::DEBUG) << "started creating element";
    created = true;
    createBoundingBox();
    addExtraPoints();

    Node* node = findStartNode(bottomLeft);

    if (node != nullptr){
        createMainPolygonUpdated(node);
        createBorderPolygon(node, node);
    } else{
        LOG(LogLevel::INFO) << "no border polygons created";
        createMainPolygonUpdated(bottomLeft);
    }

    LOG(LogLevel::DEBUG) << "element created";

    LOG::NewLine(LogLevel::INFO);

    unsigned int numberPolygonsIndices = polygonsIndices.size();
    LOG(LogLevel::INFO) << "number of polygons: " << numberPolygonsIndices;
    for (unsigned int i = 0; i < numberPolygonsIndices; i++){
        std::vector<unsigned int>& ind = *polygonsIndices[i];
        LOG l = LOG(LogLevel::INFO);
        l << "poly " << i << ") ";
        for (unsigned int j = 0; j < ind.size(); j++){
            l << ind[j] << " ";
        }
    }

    return polygonsIndices;
}

const std::vector<Vector2f>& Element::getPoints() const{
    if (!created){
//        LOG(LogLevel::WARN) << "Element not created when trying to get points";
    }
    return points;
}

const std::vector<std::shared_ptr<std::vector<unsigned int>>>& Element::getPolygonsIndices() const{
    if (!created){
        throw std::runtime_error("Element not created when trying to get indices");
    }
    return polygonsIndices;
}

const std::vector<unsigned int> Element::getStartingIndices() const{
    return indices;
}

float Element::getWidth() const{
    return width;
}

float Element::getHeight() const{
    return height;
}

float Element::getXMin() const{
    return xMin;
}

float Element::getYMin() const{
    return yMin;
}

const double Element::TOLERANCE = 1.0E-4;

void Element::createBoundingBox(){
    std::vector<unsigned int> indicesRight  = {0};
    std::vector<unsigned int> indicesLeft   = {0};
    std::vector<unsigned int> indicesTop    = {0};
    std::vector<unsigned int> indicesBottom = {0};
    for (unsigned int i = 0; i < numberStartIndices; i++){
        if (indices[i] == 0){
            i++;
        }
        unsigned int index = indices[i];
        const float& x = vertices[index].x;
        const float& y = vertices[index].y;

        if (x > vertices[indicesRight[0]].x + Element::TOLERANCE){
            indicesRight.clear();
            indicesRight.push_back(index);
        } else if (abs(x - vertices[indicesRight[0]].x) < Element::TOLERANCE){
            indicesRight.push_back(index);
        }
        if (x < vertices[indicesLeft[0]].x - Element::TOLERANCE){
            indicesLeft.clear();
            indicesLeft.push_back(index);
        } else if (abs(x - vertices[indicesLeft[0]].x) < Element::TOLERANCE){
            indicesLeft.push_back(index);
        }
        if (y > vertices[indicesTop[0]].y + Element::TOLERANCE){
            indicesTop.clear();
            indicesTop.push_back(index);
        } else if (abs(y - vertices[indicesTop[0]].y) < Element::TOLERANCE){
            indicesTop.push_back(index);
        }
        if (y < vertices[indicesBottom[0]].y - Element::TOLERANCE){
            indicesBottom.clear();
            indicesBottom.push_back(index);
        } else if (abs(y - vertices[indicesBottom[0]].y) < Element::TOLERANCE){
            indicesBottom.push_back(index);
        }
    }

//    LDEB << "indicesLeft:";
//    printDebugArray(indicesLeft);
//    LDEB << "indicesRight:";
//    printDebugArray(indicesRight);
//    LDEB << "indicesTop:";
//    printDebugArray(indicesTop);
//    LDEB << "indicesBottom:";
//    printDebugArray(indicesBottom);

    left    = vertices[indicesLeft[0]].x;
    right   = vertices[indicesRight[0]].x;
    top     = vertices[indicesTop[0]].y;
    bottom  = vertices[indicesBottom[0]].y;

    width   = right - left;
    height  = top - bottom;
    xMin    = left;
    yMin    = bottom;

    int indexLeftBottom = getOneEqual(indicesLeft, indicesBottom);
    if (indexLeftBottom < 0){
//        LDEB << "created left bottom";
        points.push_back({left, bottom});
        bottomLeft  = new Node(numberStartIndices + numberAddedVertices);
        bottomLeft->touched = true;
        numberAddedVertices++;
    } else{
        bottomLeft  = new Node(indexLeftBottom);
    }
    int indexRightBottom = getOneEqual(indicesRight, indicesBottom);
    if (indexRightBottom < 0){
//        LDEB << "created right bottom";
        points.push_back({right, bottom});
        bottomRight = new Node(numberStartIndices + numberAddedVertices);
        bottomRight->touched = true;
        numberAddedVertices++;
    } else{
        bottomRight = new Node(indexRightBottom);
    }
    int indexRightTop = getOneEqual(indicesRight, indicesTop);
    if (indexRightTop < 0){
//        LDEB << "created right top";
        points.push_back({right, top});
        topRight    = new Node(numberStartIndices + numberAddedVertices);
        topRight->touched = true;
        numberAddedVertices++;
    } else{
        topRight    = new Node(indexRightTop);
    }
    int indexLeftTop = getOneEqual(indicesLeft, indicesTop);
    if (indexLeftTop < 0){
//        LDEB << "created left top";
        points.push_back({left, top});
        topLeft     = new Node(numberStartIndices + numberAddedVertices);
        topLeft->touched = true;
        numberAddedVertices++;
    } else{
        topLeft     = new Node(indexLeftTop);
    }

    topRight->next = topLeft;
    topLeft->previous = topRight;
    topLeft->next = bottomLeft;
    bottomLeft->previous = topLeft;
    bottomLeft->next = bottomRight;
    bottomRight->previous = bottomLeft;
    bottomRight->next = topRight;
    topRight->previous = bottomRight;
}

void Element::addExtraPoints(){
    Node* start = nullptr;
    Node* node = nullptr;

    for (unsigned int i = 0; i < numberStartIndices; i++){
        unsigned int index = indices[i];
        bool isNotBorder = false;
        Node* tmp;
        if (index == bottomLeft->getIndex()){
            tmp = bottomLeft;
        } else if (index == bottomRight->getIndex()){
            tmp = bottomRight;
        } else if (index == topRight->getIndex()){
            tmp = topRight;
        } else if (index == topLeft->getIndex()){
            tmp = topLeft;
        } else{
            tmp = new Node(index);
            isNotBorder = true;
        }
        if (node != nullptr){
            node->up = tmp;
            tmp->down = node;
        } else{
            start = tmp;
        }
        node = tmp;
        if (!isNotBorder){
            continue;
        }
        const Vector2f vec = points[index];
        if (abs(vec.x - right) < Element::TOLERANCE){
            add(topRight, node, BACKWARD, SMALLER, Y_CONSIDER);
        } else if (abs(vec.x - left) < Element::TOLERANCE){
            add(bottomLeft, node, BACKWARD, BIGGER, Y_CONSIDER);
        } else if (abs(vec.y - top) < Element::TOLERANCE){
            add(topRight, node, FORWARD, SMALLER, X_CONSIDER);
        } else if (abs(vec.y - bottom) < Element::TOLERANCE){
            add(bottomLeft, node, FORWARD, BIGGER, X_CONSIDER);
        }
    }

    if (start == nullptr){
        LOG(LogLevel::ERROR) << "Start is nullptr";
        return;
    }
    start->down = node;
    node->up = start;

    node = start;
    do{
        unsigned int index = node->getIndex();
        if (index == bottomLeft->getIndex() || index == bottomRight->getIndex() ||
            index == topRight->getIndex()   || index == topLeft->getIndex()){
            node = node->up;
            continue;
        }

        const Vector2f vec = points[index];
        if (abs(vec.x - right) < Element::TOLERANCE){
            tryAdding({vec.x - width, vec.y}, bottomLeft, BACKWARD, BIGGER, Y_CONSIDER);
        } else if (abs(vec.x - left) < Element::TOLERANCE){
            tryAdding({vec.x + width, vec.y}, topRight, BACKWARD, SMALLER, Y_CONSIDER);
        } else if (abs(vec.y - top) < Element::TOLERANCE){
            tryAdding({vec.x, vec.y - height}, bottomLeft, FORWARD, BIGGER, X_CONSIDER);
        } else if (abs(vec.y - bottom) < Element::TOLERANCE){
            tryAdding({vec.x, vec.y + height}, topRight, FORWARD, SMALLER, X_CONSIDER);
        }
        node = node->up;
    } while (node != start);

    LOG::NewLine(LogLevel::INFO);
    LOG(LogLevel::INFO) << "points after adding extra points";
    for (unsigned int i = 0; i < points.size(); i++){
        LOG(LogLevel::INFO) << i << " x: " << points[i].x << " y: " << points[i].y;
    }
    LOG::NewLine(LogLevel::INFO);
}

void Element::tryAdding(const Vector2f& vector, Node* start, bool forward, bool bigger, bool x){
    Node* node;
    float px = vector.x;
    float py = vector.y;
    if (forward){
        node = start->next;
    } else{
        node = start->previous;
    }
    if (x){
        if (bigger){
            while(px > points[node->getIndex()].x + Element::TOLERANCE){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        } else{
            while(px < points[node->getIndex()].x - Element::TOLERANCE){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        }
        if (abs(px - points[node->getIndex()].x) < Element::TOLERANCE){
            return;
        }
    } else{
        if (bigger){
            while(py > points[node->getIndex()].y + Element::TOLERANCE){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        } else{
            while(py < points[node->getIndex()].y - Element::TOLERANCE){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        }
        if (abs(py - points[node->getIndex()].y) < Element::TOLERANCE){
            return;
        }
    }

    points.push_back(vector);
    Node* create = new Node(numberStartIndices + numberAddedVertices);
    numberAddedVertices++;
    if (forward){
        Node* previous = node->previous;
        previous->next = create;
        create->previous = previous;
        create->next = node;
        node->previous = create;
    } else{
        Node* next = node->next;
        next->previous = create;
        create->next = next;
        create->previous = node;
        node->next = create;
    }
    if (!create->next->touched && !create->previous->touched &&
        (abs(points[create->next->getIndex()].x - points[create->previous->getIndex()].x) < Element::TOLERANCE ||
         abs(points[create->next->getIndex()].y - points[create->previous->getIndex()].y) < Element::TOLERANCE)){
             create->previous->up = create;
             create->down = create->previous;
             create->up = create->next;
             create->next->down = create;
    } else{
        create->touched = true;
    }
}

void Element::add(Node* start, Node* create, bool forward, bool bigger, bool x){
    unsigned int index = create->getIndex();
    Node* node;
    if (forward){
        node = start->next;
    } else{
        node = start->previous;
    }
    if (x){
        float px = points[index].x;
        if (bigger){
            while(px > points[node->getIndex()].x){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        } else{
            while(px < points[node->getIndex()].x){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        }
    } else{
        float py = points[index].y;
        if (bigger){
            while(py > points[node->getIndex()].y){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        } else{
            while(py < points[node->getIndex()].y){
                if (forward){
                    node = node->next;
                } else{
                    node = node->previous;
                }
            }
        }
    }
    if (forward){
        Node* previous = node->previous;
        previous->next = create;
        create->previous = previous;
        create->next = node;
        node->previous = create;
    } else{
        Node* next = node->next;
        next->previous = create;
        create->next = next;
        create->previous = node;
        node->next = create;
    }
}

void Element::createMainPolygonUpdated(Node* start){
    std::shared_ptr<std::vector<unsigned int>> mainIndices = std::make_shared<std::vector<unsigned int>>();
    polygonsIndices.push_back(mainIndices);

    Node* node = start;
    do{
        mainIndices->push_back(node->getIndex());
        node = node->up;
    } while (node != start);
}

void Element::createBorderPolygon(Node* start, Node* begin){
    if (start->touched){
        LOG(LogLevel::ERROR) << start->getIndex() << " starting node is not part of the main polygon";
        return;
    }
    LOG(LogLevel::INFO) << "begin node: " << begin->getIndex();
    LOG(LogLevel::INFO) << "start node: " << start->getIndex();

    while (start->next == start->up){
        LOG(LogLevel::INFO) << "at " << start->getIndex();
        start = start->next;
        if (start == begin){
            LOG(LogLevel::INFO) << "arrived at begin";
            return;
        }
    }

    LOG(LogLevel::INFO) << "real starting node: " << start->getIndex();

    std::shared_ptr<std::vector<unsigned int>> borderPoly = std::make_shared<std::vector<unsigned int>>();
    polygonsIndices.push_back(borderPoly);
    borderPoly->push_back(start->getIndex());

    Node* node = start->next;
    LOG(LogLevel::INFO) << "adding border nodes: " << node->getIndex();
    while (node->touched){
        borderPoly->push_back(node->getIndex());
        node = node->next;
        LOG(LogLevel::INFO) << "adding border nodes: " << node->getIndex();
    }
    borderPoly->push_back(node->getIndex());

    if (node != begin){
        createBorderPolygon(node, begin);
    }

    node = node->down;
    LOG(LogLevel::INFO) << "adding nodes inside polygon: " << node->getIndex();
    while (node != start){
        borderPoly->push_back(node->getIndex());
        node = node->down;
        LOG(LogLevel::INFO) << "adding nodes inside polygon: " << node->getIndex();
    }
}

Node* Element::findStartNode(Node* start){
    Node* node = start;

    if (node->touched){
        while (node->touched){
            node = node->next;
        }
    } else{
        while (node->next == nullptr){
            node = node->up;
            if (node == start){
                return nullptr;
            }
        }
        while (!node->next->touched && node->next == node->up){
            node = node->next;
            if (node == start){
                return nullptr;
            }
        }
    }
    return node;
}

int Element::getOneEqual(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b){
    unsigned int sizeA = a.size();
    unsigned int sizeB = b.size();
    for (unsigned int i = 0; i < sizeA; i++){
        for (unsigned int j = 0; j < sizeB; j++){
            if (a[i] == b[j]){
                return a[i];
            }
        }
    }
    return -1;
}

void Element::printDebugArray(const std::vector<unsigned int>& a){
    unsigned int sizeA = a.size();
    LOG l = LOG(LogLevel::DEBUG);
    for (unsigned int i = 0; i < sizeA; i++){
        l << a[i] << "  ";
    }
}
