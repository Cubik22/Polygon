#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include "Logger.h"


enum class NodeType{
    // dot product is positive
    Positive = 0,
    // dot product is negative
    Negative = 1,
    // parallel node and the next parallel node is outside
};

class Node{

public:
    Node(unsigned int _index);
    ~Node();

    Node(const Node&)                 = delete;
    Node(Node&&) noexcept             = delete;
    Node& operator=(const Node&)      = delete;
    Node& operator=(Node&&) noexcept  = delete;

    mutable bool touched;

    // every node
    Node* next;
    Node* previous;
    //just for parallel nodes
    Node* up;
    Node* down;

    unsigned int getIndex() const;

    bool isIntersection() const;

    static void PrintNetwork(const Node* start, LogLevel level = LogLevel::INFO);
    static void DeleteNetwork(Node* start);

private:
    unsigned int index;
};

#endif // NETWORK_H
