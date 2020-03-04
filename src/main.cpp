#include <iostream>
#include <list>
#include <iomanip>
#include <chrono>
#include <map>

#include "main.h"
#include "../external/stb_image.h"

// TEMP
#include <windows.h>

int main(int argc, char *argv[]) {
    IParser parse(argv[1]);
}

IParser::IParser(const char *path): img(stbi_load(path, &width, &height, &channels, 0)) {
    if (img == NULL) {
        std::cout << "Error: Failed to load image: " << stbi_failure_reason() << std::endl;
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
    moveHorzSqr = channels * (boxDimensions + 2);

    parseBox(i, 0, 0, 2);

    std::cout << "Sequencing complete(?)" << std::endl;
}

void IParser::parseBox(const int &i, const int &x, const int &y, const short &direction) {

    if (addLocation(x, y, new Node(hasNumber(i)))) {
        // debug_printCoords(i);
        // Sleep(1000);
        if (!pixelColorEq(i + moveHorzSqr)) { // East
            // std::cout << "Add " << moveHorzSqr << std::endl;
            parseBox(i + moveHorzSqr, x+1, y, 0);
        }
        if (!pixelColorEq(i + movVertSqr)) { // South
            // std::cout << "Add " << movVertSqr << std::endl;
            parseBox(i + movVertSqr, x, y+1, 1);
        }
        if (!pixelColorEq(i - moveHorzSqr)) { // West
            // std::cout << "Sub " << moveHorzSqr << std::endl;
            parseBox(i - moveHorzSqr, x-1, y, 2);
        }
        if (!pixelColorEq(i - movVertSqr)) { // North
            // std::cout << "Sub " << movVertSqr << std::endl;
            parseBox(i - movVertSqr, x, y-1, 3);
        }
    } /* else { // If this square was allready added, to prevent recursing out of control, just continue to end of block
        switch (direction) {
            case 0: // East
                if (pixelColorEq(i + boxDimensions)) {
                    parseBox(i + boxDimensions, x+1, y, 0);
                }
                break;
            case 1: // South
                if (pixelColorEq(i + movVertSqr)) {
                    parseBox(i + movVertSqr, x, y+1, 1);
                }
                break;
            case 2: // West
                if (pixelColorEq(i - boxDimensions)) {
                    parseBox(i - boxDimensions, x-1, y, 2);
                }
                break;
            case 3: // North
                if (pixelColorEq(i - movVertSqr)) {
                    parseBox(i + boxDimensions, x, y-1, 3);
                }
                break;
            default: throw "Line 107 done messed up";
        }
    }
    */
    // And well I guess that's the end of it


    // if (vertical) {
    //     std::list<Node> list;
    //     for (;; i += movVertSqr) {
    //         if () {

    //         }
    //     }
    // } else {

    // }
}

bool IParser::addLocation(int x, int y, Node n) {
    // std::cout << "COords: (" << x << ", " << y << ")" << std::endl;
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
    // std::cout << "------------------------------------------" << std::endl;
    // std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
    // std::cout << "------------------------------------------" << std::endl;
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
    // std::cout << "------------------------------------------" << std::endl;
    // std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
    // std::cout << "------------------------------------------" << std::endl;
    for (int c = 0, r = 0;;c++) {
        if (c >= boxDimensions) {
            c = 0;
            r++;
            if (r >= boxDimensions) {
                break;
            }
        }
        const int loc = i - c * channels + r * movVert;
        // std::cout << "~~~~~~~~~~~~~~~~~~~~~";
        // std::cout << c << std::endl;
        // std::cout << (loc / channels) << std::endl;
        // std::cout << ((loc / channels) % width) << ", " << ((loc / channels) / width) << std::endl;
        // map[boxDimensions-1-c][r] = (img[loc] << 16) | (img[loc+1] << 8) | img[loc+2];

        if ((img[loc] << 16) | (img[loc+1] << 8) | img[loc+2] != 0xFFFFFF) {
            return true;
        }
    }
    return false;
}

bool IParser::pixelColorEq(const int &index) {
    return index < 0 || index >= size || (r == img[index] && b == img[index+1] && g == img[index+2]);
}

bool IParser::pixelColorEq(const int &index, const int val) {
    return index < 0 || index >= size || (val == img[index] && val == img[index+1] && val == img[index+2]);
}

void IParser::debug_printCoords(int i) {
    std::cout << "(" << ((i / channels) % width) << ", " << ((i / channels) / width) << ")" << std::endl;
}

Node::Node(const bool &hasNumber): hasNumber(hasNumber) {
}
