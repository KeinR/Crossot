#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <map>

#define BOX_WIDTH 18
// +1
#define BOX_DIMENSIONS 17

typedef const unsigned char* raw;

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

struct Handle;

class Data {
    public:
    bool addEntree(const Handle &handle, const int &i, const node &n);
    private:
    std::map<const int, node> data; // Potential bug: int values aren't kept track of properly, in wich case would need to keep track of switches indiv
};

struct Handle {
    const unsigned char *const &img;
    const int &width;
    const int &height;
    const int &channels;
    const int channelsM1;
    const int movVert;
    const int movVertSqr;
    const int size;
    // Background colors
    const unsigned char &r;
    const unsigned char &b;
    const unsigned char &g;
    // Mutable data
    const Data data;
};

void parseSect(const Handle &handle, int i, bool vertical);
int parseNumber(const Handle &handle, int i);
bool pixelColorEq(const Handle &handle, const int &index);
bool pixelColorEq(const Handle &handle, const int &index, const int val);

#endif
