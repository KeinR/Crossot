#include <vector>
#include <iostream>

#include "Node.h"

Node::Node(const char &hasNumber): doesHaveNumber(hasNumber)  {
}

Node::Node() {
}

bool Node::hasLeNumber() {
    return doesHaveNumber & 0x1;
}
bool Node::isHorizontal() {
    return doesHaveNumber & 0x2;
}
bool Node::isVertical() {
    return doesHaveNumber & 0x4;
}

void Node::addParent(std::vector<Node*> &p) {
    parents[parentNum++] = &p;
}

bool Node::hasTwoParents() {
    return parentNum > 1;
}

std::vector<Node*> &Node::getParentA() {
    if (parentNum < 1) {
        std::cout << "FUCK IT'S TOO MUCH, A!" << std::endl;
        std::cout << "WE'RE GONNA' SEG FAULT!" << std::endl;
        std::cout << "ABORT ABORT" << std::endl;
        abort();
    }
    return *parents[0];
}

std::vector<Node*> &Node::getParentB() {
    if (parentNum < 2) {
        std::cout << "FUCK IT'S TOO MUCH, B!" << std::endl;
        std::cout << "WE'RE GONNA' SEG FAULT!" << std::endl;
        std::cout << "ABORT ABORT" << std::endl;
        abort();
    }
    return *parents[1];
}
