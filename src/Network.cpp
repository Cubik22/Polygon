#include "Network.h"
#include "Logger.h"
#include <iostream>

Node::Node(unsigned int _index) : touched{false}, next{nullptr}, previous{nullptr}, up{nullptr}, down{nullptr}, index{_index} {}

Node::~Node() {}

unsigned int Node::getIndex() const{
    return index;
}

Network::Network() {}

void Network::printNetwork(const Node* start){
    const Node* node = start;
    unsigned int i = 0;
    do{
        if (node == nullptr){
            LOG(LogLevel::WARN) << i << ": is nullptr";
            return;
        }
        LOG(LogLevel::INFO) << i++ << ": " << node->getIndex();
        node = node->next;
    } while (node != start);
}

void Network::deleteNetwork(Node *start){
    Node* node;
    Node* tmp;
    if (start != nullptr){
        node = start->next;
    } else{
        LOG(LogLevel::ERROR) << "problems when deleting node network, start node is nullptr";
        return;
    }
    while (node != start){
        LOG(LogLevel::DEBUG) << "deleted";
        tmp = node->next;
        delete node;
        node = tmp;
    }
    delete start;
}

