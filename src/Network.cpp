#include "Network.h"
#include <iostream>

Node::Node(unsigned int _index) : touched{false}, next{nullptr}, previous{nullptr}, up{nullptr}, down{nullptr}, index{_index} {}

Node::~Node() {}

unsigned int Node::getIndex() const{
    return index;
}

bool Node::isIntersection() const{
    return (up || down || up != nullptr || down != nullptr);
}

bool Node::isBetween() const{
    return (!next || !previous || next == nullptr || previous == nullptr);
}

void Node::PrintNetwork(const Node* start, LogLevel level){
    const Node* node = start;
    unsigned int i = 0;
    do{
        if (node == nullptr){
            LOG(LogLevel::WARN) << i << ": is nullptr";
            return;
        }
        LOG(level) << i++ << ": " << node->getIndex();
        node = node->next;
    } while (node != start);
}

void Node::DeleteNetwork(Node *start){
    Node* node;
    Node* tmp;
    if (start != nullptr){
        node = start->next;
    } else{
        LOG(LogLevel::ERROR) << "problems when deleting node network, start node is nullptr";
        return;
    }
    while (node != start){
        //LOG(LogLevel::DEBUG) << "deleted";


        tmp = node->next;
        delete node;
        node = tmp;
    }
    delete start;
}

