#include <iostream>
#include <list>
#include <map>
#include <cmath>
#include <vector>

#include "Node.h"
#include "main.h"
#include "../external/stb_image.h"

#include "IParser.h"

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
    unsigned int number = 1; // TEMP
    std::vector<Node*> seq[totalAns]; // INT
    int seqLength = 0; // INT
    // std::vector<Node> *temp;
    for (int y = locatorMinY; y <= locatorMaxY; y++) {
        for (int x = locatorMinX; x <= locatorMaxX; x++) {
            if (locatorMap.count(x) && locatorMap[x].count(y)) {
                if (locatorMap[x][y]->hasLeNumber()) {
                    if (number <= 9) {
                        std::cout << (unsigned char)(number + 48);
                    } else {
                        std::cout << (unsigned char)(number + 72);
                    }
                    debugFile << "NUMBER: " << number << std::endl;
                    number++;

                    // Should only seg fault if search code gets funky,
                    // or if I'm using std::move wrong
                    std::vector<Node*> &vec = seq[seqLength++];
                    vec.push_back(locatorMap[x][y]);
                    if (locatorMap[x][y]->isHorizonal()) {
                        for (int tx = x+1; locatorMap.count(tx) && locatorMap[tx].count(y); tx++) {
                            vec.push_back(locatorMap[tx][y]);
                        }
                    } else {
                        std::map<int, Node*> &mapXCache = locatorMap[x];
                        for (int ty = y+1; mapXCache.count(ty); ty++) {
                            vec.push_back(locatorMap[x][ty]);
                        }
                    }
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
        std::list<std::string> temp;
        std::ifstream answers(answerPath);
        std::string buff;
        // A little scheme to avoid superfluous iteratons when sorting: get min length beforehand
        int length = 0xFFFFFF;
        for (char c; !answers.eof();) {
            answers.get(c);
            switch (c) {
                case ' ':
                case '\n':
                case '\r':
                case ',':
                    if (buff.length() > 0) {
                        if (buff.length() < length) {
                            length = buff.length();
                        }
                        temp.push_back(buff);
                        buff.clear();
                    }
                    break;
                default:
                    buff += c;
            }
        }
        answers.close();
        if (buff.length() > 0) {
            temp.push_back(std::move(buff));
        }
        // Insertion sort, by size
        for (; temp.size() != 0; length++) {
            for (std::list<std::string>::iterator it = temp.begin(); it != temp.end();) {
                if (it->length() == length) {
                    ans.push_back(std::move(*it));
                    it = temp.erase(it);
                } else {
                    ++it;
                }
            }
        }

        for (std::string &s : ans) {
            std::cout << s << std::endl;
        }
    }

    /*
    std::list<std::string>::iterator unique = ans.begin();
    std::list<std::string>::iterator bEnd = ans.end();
    bEnd--;
    for (std::list<std::string>::iterator it = ans.begin(), start = ans.begin(), end = ans.end(); it != end;) {
        const int length = it->length();
        if (it != bEnd) {
            ++it;
            if (length == it->length()) {
                // Skip dupes, obv not uniq
                while (++it != end && length == it->length());
                continue;
            }
            --it;
        }
        unique = it;
        break;
    }
    std::cout << "Unique esta " << *unique << std::endl;
    // ans.erase(unique);

    // Abstract resulting data

    for (int i = 0; i < seqLength; i++) {
        if (unique->length() == seq[i].size()) {
            std::cout << "Found eq, q# " << (i+1) << std::endl;
        }
    }
    */
}

void IParser::parseBox(const int &i, const int &x, const int &y, const short &direction) {

    // std::cout << "Erm..." << std::endl;
    if (i < size && i >= 0 && addLocation(x, y, new Node((char)(hasNumber(i) | (inBox(i + moveHorzSqr) << 1))))) {
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
