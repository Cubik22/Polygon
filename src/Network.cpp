#include "Network.h"
#include <iostream>

Node::Node(unsigned int _index) : touched{false}, next{nullptr}, previous{nullptr}, up{nullptr}, down{nullptr}, index{_index} {}

Node::~Node(){
    if (next){
        delete next;
    }
}

unsigned int Node::getIndex() const{
    return index;
}

Network::Network(){

}

void Network::printNetwork(const Node* start){
    const Node* node = start;
    unsigned int i = 0;
    do{
        if (node == nullptr){
            std::cout << i << ": is nullptr\n";
            return;
        }
        std::cout << i++ << ": " << node->getIndex() << "\n";
        node = node->next;
    } while (node != start);
}

