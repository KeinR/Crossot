#include "Node.h"

Node::Node(const char &hasNumber): doesHaveNumber(hasNumber)  {
}

Node::Node() {
}

bool Node::hasLeNumber() {
    return doesHaveNumber & 0x1;
}
bool Node::isHorizonal() {
    return doesHaveNumber & 0x2;
}
