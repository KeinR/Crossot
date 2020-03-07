#ifndef IPARSER_H_INCLUDED
#define IPARSER_H_INCLUDED

#include <map>

#include "Node.h"

#define K_REASONABLE_SKIP 8

struct error {
    // Meh
};

class IParser {
    public:
    IParser(const char *path, const char *answerPath);
    private:
    void parseBox(const int &i, const int &x, const int &y, const short &direction);
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
    std::map<int, std::map<int, Node*>> locatorMap;
    int locatorMinX = 0xFFFFFF;
    int locatorMaxX = -0xFFFFFF;
    int locatorMinY = 0xFFFFFF;
    int locatorMaxY = -0xFFFFFF;
    int totalAns = 0;
    bool addLocation(int x, int y, Node *&&n);
    bool hasNumber(int i);
    bool inBox(const int &index);
    bool pixelColorEq(const int &index, const int val);
    void debug_printCoords(int i);
};

#endif
