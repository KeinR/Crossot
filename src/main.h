#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <map>

#define K_ANSWER_BUFFER_SIZE 50
#define K_REASONABLE_SKIP 8

// typedef const unsigned char* raw;

struct error {
    // Meh
};

struct color {
    unsigned char r;
    unsigned char b;
    unsigned char g;
};

class Node {
    public:
    Node();
    Node(const bool &hasNumber);
    private:
    bool hasNumber;
    char val = '\0';
};

struct answer {
    Node *list;
    bool vertical;
};

class IParser {
    public:
    IParser(const char *path, const char *answerPath);
    private:
    void parseBox(const int &i, const int &x, const int &y, const short &direction);
    // private:
    const unsigned char *const &img;
    int width;
    int height;
    int channels;
    int channelsM1;
    int movVert;
    int movVertSqr;
    int moveHorzSqr;
    int size;
    int boxDimensions;
    int boxDimensionsScaledVertical;
    int boxDimensionsScaledHorizontal;
    // Background colors
    unsigned char r;
    unsigned char b;
    unsigned char g;
    // std::map<const int, node> data; // Potential bug: int values aren't kept track of properly, in wich case would need to keep track of switches indiv
    std::map<int, std::map<int, Node>> locatorMap;
    int locatorMinX = 0xFFFFFF;
    int locatorMaxX = -0xFFFFFF;
    int locatorMinY = 0xFFFFFF;
    int locatorMaxY = -0xFFFFFF;
    bool addLocation(int x, int y, Node n);
    bool hasNumber(int i);
    bool inBox(const int &index);
    bool pixelColorEq(const int &index, const int val);
    void debug_printCoords(int i);
};

// int lookupNumber(const std::list<int> &vertical, const std::list<int> &horizontal);

#endif
