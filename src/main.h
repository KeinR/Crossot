#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <map>

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

struct node {
    char val = '\0';
    node *nTop;
    node *nBelow;
    node *nLeft;
    node *nRight;
};

struct answer {
    node *list;
    bool vertical;
};

class IParser {
    public:
    IParser(const char *path);
    private:
    void parseSect(int i, bool vertical);
    // private:
    const unsigned char *const &img;
    int width;
    int height;
    int channels;
    int channelsM1;
    int movVert;
    int movVertSqr;
    int size;
    int boxDimensions;
    // Background colors
    unsigned char r;
    unsigned char b;
    unsigned char g;
    std::map<const int, node> data; // Potential bug: int values aren't kept track of properly, in wich case would need to keep track of switches indiv
    bool addEntree(const int &i, const node &n);
    int parseNumber(int i);
    bool pixelColorEq(const int &index);
    bool pixelColorEq(const int &index, const int val);
    void debug_printCoords(int i);
};

int lookupNumber(const std::list<int> &vertical, const std::list<int> &horizontal);

#endif
