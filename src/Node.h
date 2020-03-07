#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

class Node {
    public:
    Node();
    Node(const char &hasNumber);
    char val = '\0';
    bool hasLeNumber();
    bool isHorizonal();
    private:
    char doesHaveNumber;
};

#endif
