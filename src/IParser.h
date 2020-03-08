#ifndef IPARSER_H_INCLUDED
#define IPARSER_H_INCLUDED

#include <map>

#include "Node.h"
#include "Solver.h"

struct error {
    // Meh
};

class IParser {
    public:
    IParser(const char *path, const char *answerPath);
    private:
    void parseBox(int i, const int &x, const int &y, const char &direction);
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
    std::map<int, std::map<int, Node>> locatorMap;
    int locatorMinX = 0xFFFFFF;
    int locatorMaxX = -0xFFFFFF;
    int locatorMinY = 0xFFFFFF;
    int locatorMaxY = -0xFFFFFF;
    int totalAns = 0;
    bool addLocation(int x, int y, const int &i);
    bool hasNumber(int i);
    bool inBox(int index);
    bool pixelColorEq(const int &index, const int val);
    void debug_printCoords(int i);
    // two bits, 01 vertical, 11 horizontal, 00 not a starter node
    char getCanHorizontal(const int &i);
};

#endif
