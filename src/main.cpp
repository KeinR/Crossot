#include <iostream>
#include <list>
#include <iomanip>
#include <chrono>
#include <map>
#include <cmath>

#include "main.h"
#include "../external/stb_image.h"

// TEMP
#include <windows.h>

// DEBUG
#include <fstream>
std::ofstream debugFile("debug.log");

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Invalid # of arguments" << std::endl;
        return 1;
    }
    IParser parse(argv[1], argv[2]);
}

IParser::IParser(const char *path, const char *answerPath): img(stbi_load(path, &width, &height, &channels, 0)) {
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
    // debug_printCoords(f);
    i = f - movVert * 3 - channels * 3;
    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    boxDimensions = 1;
    for (; pixelColorEq(f -= channels, 0xFF); boxDimensions++);

    std::cout << "(" << ((f / channels) % width) << ", " << ((f / channels) / width) << ")" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    std::cout << "DIMENSIONS: " << boxDimensions << std::endl;

    movVertSqr = movVert * (boxDimensions + 2);
    moveHorzSqr = channels * (boxDimensions + 2);

    boxDimensionsScaledVertical = movVert * boxDimensions;
    boxDimensionsScaledHorizontal = channels * boxDimensions;

    // i += channels * 5;
    // debug_printCoords(i);
    parseBox(i, 0, 0, 2);

    std::cout << "Sequencing complete(?)" << std::endl;
    std::cout << "Printing>..." << std::endl;
    // const int lengthX = std::abs(locatorMaxX - locatorMinX);
    // const int lengthY = std::abs(locatorMaxY - locatorMinY);
    // int res[lengthX][lengthY];
    unsigned int number = 1;
    int refLength = 0;
    answer ref[totalAns];
    for (int y = locatorMinY; y <= locatorMaxY; y++) {
        for (int x = locatorMinX; x <= locatorMaxX; x++) {
            if (locatorMap.count(x) && locatorMap[x].count(y)) {
                if (locatorMap[x][y]->doesHaveNumber) {
                    if (number <= 9) {
                        std::cout << (unsigned char)(number + 48);
                    } else {
                        std::cout << (unsigned char)(number + 72);
                    }
                    debugFile << "NUMBER: " << number << std::endl;
                    number++;
                } else {
                    std::cout << "0";
                }
                // res[x][y] = 1;
            } else {
                std::cout << " ";
                // res[x][y] = 0;
            }
        }
        std::cout << std::endl;
    }

    std::list<std::string> ans;
    {
        std::ifstream answers(answerPath);
        std::string buff;
        for (char c; !answers.eof();) {
            answers.get(c);
            switch (c) {
                case ' ':
                case '\n':
                case '\r':
                case ',':
                    if (buff.length() > 0) {
                        ans.push_back(buff);
                        buff.clear();
                    }
                    break;
                default:
                    buff += c;
            }
        }
        if (buff.length() > 0) {
            ans.push_back(std::move(buff));
        }
        answers.close();
    }

    std::list<std::string>::iterator unique = ans.begin();
    for (std::list<std::string>::iterator it = ans.begin(), end = ans.end(); ++it != end;) {
        if (unique->length() == it->length()) {
            ++it;
            if (it == end) {
                std::cout << "Fuk no match" << std::endl;
                exit(101);
            }
            unique = it;
        }
    }
    std::cout << "Unique esta " << *unique << std::endl;
    ans.erase(unique);

    // Abstract resulting data

}

void IParser::parseBox(const int &i, const int &x, const int &y, const short &direction) {

    // std::cout << "Erm..." << std::endl;
    if (i < size && i >= 0 && addLocation(x, y, new Node(hasNumber(i)))) {
        debug_printCoords(i);
        // Sleep(1000);
        // std::cout << "Hello?" << std::endl;
        debugFile << "Pricess A" << std::endl;
        if (inBox(i + moveHorzSqr)) { // East
            // std::cout << "Add " << moveHorzSqr << std::endl;
            debugFile << "Allowing EAST with i = " << (i + moveHorzSqr) << std::endl;
            parseBox(i + moveHorzSqr, x+1, y, 0);
        }
        debugFile << "Pricess B:" << std::endl;
        debug_printCoords(i + movVertSqr);
        if (inBox(i + movVertSqr)) { // South
            // std::cout << "Add " << movVertSqr << std::endl;
            debugFile << "Allowing SOUTH with i = " << (i + movVertSqr) << std::endl;
            // std::cout << "MOV SOUTH" << std::endl;
            parseBox(i + movVertSqr, x, y+1, 1);
        }
        debugFile << "Pricess C" << std::endl;
        if (inBox(i - moveHorzSqr)) { // West
            // std::cout << "Sub " << moveHorzSqr << std::endl;
            debugFile << "Allowing WEST with i = " << (i - moveHorzSqr) << std::endl;
            parseBox(i - moveHorzSqr, x-1, y, 2);
        }
        debugFile << "Pricess D" << std::endl;
        if (inBox(i - movVertSqr)) { // North
            // std::cout << "Sub " << movVertSqr << std::endl;
            debugFile << "Allowing NORTH with i = " << (i - movVertSqr) << std::endl;
            parseBox(i - movVertSqr, x, y-1, 3);
        }
    } else {
        debugFile << "DENIED..." << std::endl;
        // debugFile << "DENIED, i=" << i << std::endl;
        // debug_printCoords(i);
        // debugFile << "---" << std::endl;
        // std::cout << "deneid" << std::endl;
    }
    // And well I guess that's the end of it
}

bool IParser::addLocation(int x, int y, Node *&&n) {
    // std::cout << "COords: (" << x << ", " << y << ")" << std::endl;
    if (x < locatorMinX) {
        locatorMinX = x;
    }
    if (x > locatorMaxX) {
        locatorMaxX = x;
    }
    if (y < locatorMinY) {
        locatorMinY = y;
    }
    if (y > locatorMaxY) {
        locatorMaxY = y;
    }

    if (!locatorMap.count(x)) {
        std::map<int, Node*> leMap;
        leMap.emplace(y, n);
        locatorMap.emplace(x, std::move(leMap));
        totalAns++;
        return true;
    } else {
        std::map<int, Node*> &leMap = locatorMap[x];
        if (!leMap.count(y)) {
            leMap.emplace(y, n);
            totalAns++;
            return true;
        }
    }
    debugFile << "Denied ALL " << std::endl;
    return false;
}

bool IParser::hasNumber(int i) {
    
    // Move to bottom right
    for (; pixelColorEq(i, 0xFF); i += channels) {
        if (i >= size) {
            return false;
        }
    }
    i -= channels;
    for (; pixelColorEq(i, 0xFF); i += movVert) {
        if (i >= size) {
            return false;
        }
    }
    i -= movVert;

    // Now move up and see the rise and fall of mount
    int last = i;
    for (; pixelColorEq(last, 0xFF); last -= channels) {
        if (last < 0) {
            return false;
        }
    }
    last = last % width;

    for (; pixelColorEq(i, 0xFF); i -= movVert) {
        if (i < 0) {
            return false;
        }
        int f = i;
        for (; pixelColorEq(f, 0xFF); f -= channels) {
            if (f < 0) {
                return false;
            }
        }
        if (f % width != last) {
            debugFile << channels << std::endl;
            debugFile << movVert << std::endl;
            debugFile << "RISE DIFF " << std::abs((f % width) - last) << std::endl;
            debug_printCoords(f % width);
            debug_printCoords(last);
            debugFile << "----------------" << std::endl;
            return true;
        }
    }

    /*

    for (;; i -= movVert) {
        if (i < 0) {
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            i += movVert;
            break;
        }
    }
    // std::cout << "~A" << std::endl;
    for (;; i += channels) {
        if (i >= size) {
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            i -= channels;
            break;
        }
    }
    // std::cout << "~B" << std::endl;
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

        if (loc < 0 || loc + 2 >= size) {
            return false;
        }

        if ((img[loc] << 16) | (img[loc+1] << 8) | img[loc+2] != 0xFFFFFF) {
            // std::cout << "Finisjo" << std::endl;
            return true;
        }
    }
    // std::cout << "Funisjo" << std::endl;
    */
    return false;
}

bool IParser::inBox(const int &index) {
    // std::cout << "Run" << std::endl;
    // std::cout << "1" << std::endl;
    if (!pixelColorEq(index, 0xFF)) {
        debugFile << "Was I wrong?" << std::endl;
        debug_printCoords(index);
        return false;
    }
    for (int i = index;; i += movVert) {
        if (i >= size) {
            debugFile << ">> end 0" << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            if (std::abs(index - i) > boxDimensionsScaledVertical) {
                // std::cout << "Return" << std::endl;
                debugFile << ">> end A, " << std::abs(index - i) << std::endl;
                return false;
            } else {
                break;
            }
        }
    }
    // std::cout << "2" << std::endl;
    for (int i = index;; i -= movVert) {
        if (i < 0) {
            debugFile << ">> end B" << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            if (std::abs(index - i) > boxDimensionsScaledVertical) {
                debugFile << ">> end C" << std::endl;
                return false;
            } else {
                break;
            }
        }
    }
    // std::cout << "3" << std::endl;
    for (int i = index;; i += channels) {
        if (i >= size) {
            debugFile << ">> end D" << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            if (std::abs(index - i) > boxDimensionsScaledHorizontal) {
                debugFile << ">> end E" << std::endl;
                return false;
            } else {
                break;
            }
        }
    }
    // std::cout << "4" << std::endl;
    for (int i = index;; i -= channels) {
        if (i < 0) {
            debugFile << ">> end F" << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            if (std::abs(index - i) > boxDimensionsScaledHorizontal) {
                debugFile << ">> end G" << std::endl;
                return false;
            } else {
                break;
            }
        }
    }
    // std::cout << "Stop" << std::endl;
    debugFile << ">> end H" << std::endl;
    return true;
    // return index < 0 || index >= size || (r == img[index] && b == img[index+1] && g == img[index+2]);
}

bool IParser::pixelColorEq(const int &index, const int val) {
    return index < 0 || index >= size || (val == img[index] && val == img[index+1] && val == img[index+2]);
}

void IParser::debug_printCoords(int i) {
    debugFile << "(" << ((i / channels) % width) << ", " << ((i / channels) / width) << "), " << i << ", len = " << size << std::endl;
}

Node::Node(const bool &hasNumber): doesHaveNumber(hasNumber) {
    debugFile << "And as it should be, it is set to " << (hasNumber ? "TRUE" : "FALSE") << std::endl;
}

Node::Node() {
}
