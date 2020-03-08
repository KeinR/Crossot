#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>

class Node {
    public:
    Node();
    Node(const char &hasNumber);
    int changeId = 0; // Recursion ID of the one that modded val
    char val;
    bool hasLeNumber();
    bool isHorizontal();
    void addParent(std::vector<Node*> &p);
    bool hasTwoParents();
    // Reference or pointer? We'll see...
    std::vector<Node*> &getParentA();
    std::vector<Node*> &getParentB();
    private:
    std::vector<Node*> *parents[2];
    char parentNum = 0;
    char doesHaveNumber;
};

#endif
