#ifndef NETWORK_H
#define NETWORK_H

#include <vector>

enum class NodeType{
    // dot product is positive
    Positive = 0,
    // dot product is negative
    Negative = 1,
    // parallel node and the next parallel node is outside
    Single =   2,
    // parallel node and the next parallel node is inside
    Double =   3
};

class Node{
public:
    Node(unsigned int _index);
    ~Node();
    // every node
    Node* next;
    Node* previous;
    //just for parallel nodes
    Node* up;
    Node* down;
    unsigned int getIndex() const;
private:
    unsigned int index;
};

class Network{
public:
    Network();

    static void printNetwork(const Node* start);
};

#endif // NETWORK_H
