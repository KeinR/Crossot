#include <iostream>
#include <list>
#include <iomanip>
#include <chrono>
#include <map>

#include "main.h"
#include "../external/stb_image.h"

int main(int argc, char *argv[]) {
    IParser parse(argv[1]);
}

IParser::IParser(const char *path): img(stbi_load(path, &width, &height, &channels, 0)) {
    if (img == NULL) {
        std::cout << "Error: Failed to load image" << std::endl;
        error e;
        throw e;
    }
    if (width < 10 || height < 10) {
        std::cout << "Error: image size too small" << std::endl;
        error e;
        throw e;
    }

    channelsM1 = channels-1;
    movVert = width * channels;
    size = width * height * channels; // Total size of image raw
    r = img[0];
    b = img[1];
    g = img[2];

    std::cout << movVert << std::endl;
    std::cout << movVertSqr << std::endl;

    // Ignores alpha

    std::cout << "Width == " << width << std::endl;
    std::cout << "Height == " << height << std::endl;
    std::cout << "Size == " << size << std::endl;

    std::cout << "Channles " << channels << std::endl;

    std::cout << "BG = " << (int)img[0] << ", " << (int)img[1] << ", " << (int)img[2] << std::endl;

    int i = channels;
    // Find first box and determine box size
    for (; i+channelsM1 < size; i += channels) {
        if (img[i] != r || img[i+1] != b || img[i+2] != g) {
            // std::cout << "Yes" << std::endl;
            // std::cout << (i / channels) << std::endl;
            std::cout << "+++" << std::endl;
            std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
            i += K_REASONABLE_SKIP * channels;
            i += movVert * K_REASONABLE_SKIP;
            std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
            std::cout << "+++" << std::endl;
            break;
        }
    }

    std::cout << "------------------------------------------" << std::endl;
    // Find box size
    int f = i;
    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    for (; pixelColorEq(f, 0xFF); f += channels);
    f -= channels;
    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    for (; pixelColorEq(f, 0xFF); f += movVert);
    f -= movVert;
    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    boxDimensions = 1;
    for (; pixelColorEq(f -= channels, 0xFF); boxDimensions++);

    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    std::cout << "DIMENSIONS: " << boxDimensions << std::endl;

    movVertSqr = movVert * boxDimensions;

    parseSect(i, true);

    // bool vertical;
    // int iu = i + movVertSqr;
    // if (!pixelColorEq(handle, iu)) {
    //     parseSect(handle, iu, true);
    // }
    // iu = i + BOX_WIDTH;
    // if (!pixelColorEq(handle, iu)) {
    //     parseSect(handle, iu, false);
    // }
    std::cout << "Sequencing complete(?)" << std::endl;
}

void IParser::parseSect(int i, bool vertical) {
    const int start = i;
    parseNumber(i);
    // if (vertical) {
    //     std::list<node> list;
    //     for (;; i += movVertSqr) {
    //         if () {

    //         }
    //     }
    // } else {

    // }
}

bool IParser::addEntree(const int &i, const node &n) {
    if (!data.count(i)) {
        data[i] = n;
        return true;
    }
    return false;
}

int IParser::parseNumber(int i) {
    int map[boxDimensions][boxDimensions];
    std::cout << "------------------------------------------" << std::endl;
    std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    for (;; i -= movVert) {
        if (!pixelColorEq(i, 0xFF)) {
            i += movVert;
            break;
        }
    }
    for (;; i += channels) {
        if (!pixelColorEq(i, 0xFF)) {
            i -= channels;
            break;
        }
    }
    std::cout << "------------------------------------------" << std::endl;
    std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    for (int c = 0, r = 0;;c++) {
        if (c >= boxDimensions) {
            c = 0;
            r++;
            if (r >= boxDimensions) {
                break;
            }
        }
        const int loc = i - c * channels + r * movVert;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~";
        std::cout << c << std::endl;
        std::cout << (loc / channels) << std::endl;
        std::cout << ((loc / channels) % width) << ", " << ((loc / channels) / width) << std::endl;
        map[boxDimensions-1-c][r] = (img[loc] << 16) | (img[loc+1] << 8) | img[loc+2];
    }

    std::cout << std::hex;
    for (int y = 0; y < boxDimensions; y++) {
        for (int x = 0; x < boxDimensions; x++) {
            if (map[x][y] == 0xFFFFFF) {
                std::cout << "0 ";
            } else {
                std::cout << map[x][y] << " ";
                // std::cout << "1 ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::dec;

    // Vertical
    std::list<int> vertical;
    int last = 0;
    for (int x = 0; x < boxDimensions; x++) {
        int intersections = 0;
        for (int y = 0; y < boxDimensions; y++) {
            if (map[x][y] != 0xFFFFFF) {
                intersections++;
                for (; y < boxDimensions; y++) {
                    if (map[x][y] == 0xFFFFFF) {
                        break;
                    }
                }
            }
        }
        if (intersections && intersections != last) {
            vertical.push_back(last = intersections);
            std::cout << "NomA: " << intersections << std::endl;
        }
    }

    std::cout << "Horizontal:" << std::endl;

    std::list<int> horizontal;
    last = 0;
    for (int y = 0; y < boxDimensions; y++) {
        int intersections = 0;
        for (int x = 0; x < boxDimensions; x++) {
            if (map[x][y] != 0xFFFFFF) {
                intersections++;
                for (; x < boxDimensions; x++) {
                    if (map[x][y] == 0xFFFFFF) {
                        break;
                    }
                }
            }
        }
        if (intersections && intersections != last) {
            horizontal.push_back(last = intersections);
            std::cout << "NomB: " << intersections << std::endl;
        }
    }

    std::cout << "Well then : " << lookupNumber(vertical, horizontal) << std::endl;

    return 1;
}

bool IParser::pixelColorEq(const int &index) {
    return r == img[index] && b == img[index+1] && g == img[index+2];
}

bool IParser::pixelColorEq(const int &index, const int val) {
    return val == img[index] && val == img[index+1] && val == img[index+2];
}

void IParser::debug_printCoords(int i) {
    std::cout << "(" << ((i / channels) % width) << ", " << ((i / channels) / width) << ")" << std::endl;
}

int lookupNumber(const std::list<int> &vertical, const std::list<int> &horizontal) {
    int code = 0;
    int index = 1;
    for (std::list<int>::const_reverse_iterator it = horizontal.crbegin(), end = horizontal.crend(); it != end; ++it) {
        code += *it * index;
        std::cout << "CAAl " << *it << std::endl;
        index *= 10;
    }
    for (std::list<int>::const_reverse_iterator it = vertical.crbegin(), end = vertical.crend(); it != end; ++it) {
        code += *it * index;
        std::cout << "CABl " << *it << std::endl;
        index *= 10;
    }
    std::cout << "Code; " << code << std::endl;
    switch (code) {
        case 1211: return 0;
        case 11:
        case 2121: return 1;
        case 232121: return 2;
        case 311: return 3;
        case 11: return 4;
        case 321: return 5;
        case 132121: return 6;
        case 211: return 7;
        case 23212121: return 8;
        case 231121: return 9;
        default: throw "Fuk";
    }
}
