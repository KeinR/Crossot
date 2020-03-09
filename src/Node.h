#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

class Node {
    public:
    Node();
    Node(const bool &hasNumber);
    int changeId = 0; // Recursion ID of the one that modded val
    char val = '#'; // A debug thing
    bool doesHaveNumber;
};

#endif
