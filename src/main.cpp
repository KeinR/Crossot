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

void IParser::parseBox(int i, int x, int y, bool direction) {
    const int start = i;
    const bool spawn = hasNumber(i);
    const Node n = new Node(spawn);

    if (addLocation(x, y, n)) {

    }

    switch (direction) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default: throw "Line 107";
    }


    // if (vertical) {
    //     std::list<Node> list;
    //     for (;; i += movVertSqr) {
    //         if () {

    //         }
    //     }
    // } else {

    // }
}

bool IParser::addLocation(const int &x, const int &y, const Node &n) {
    if (!locatorMap.count(x)) {
        std::map<int, Node> leMap;
        leMap.emplace(y, n);
        locatorMap.emplace(x, std::move(leMap));
        return true;
    } else {
        std::map<int, Node> &leMap = locatorMap[x];
        if (!leMap.count(y)) {
            leMap.emplace(y, n);
            return true;
        }
    }
    return false;
}

bool IParser::hasNumber(int i) {
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
        // map[boxDimensions-1-c][r] = (img[loc] << 16) | (img[loc+1] << 8) | img[loc+2];

        if ((img[loc] << 16) | (img[loc+1] << 8) | img[loc+2] != 0xFFFFFF) {
            return true;
        }
    }
    return false;
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

Node::Node(const bool &hasNumber): hasNumber(hasNumber) {
}
