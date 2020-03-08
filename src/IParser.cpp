#include <iostream>
#include <list>
#include <map>
#include <cmath>
#include <vector>

#include "Node.h"
#include "Solver.h"
#include "main.h"
#include "../external/stb_image.h"

#include "IParser.h"

#define K_REASONABLE_SKIP 8

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
    i = f - movVert * 2 - channels * 2; // <-- Tweak as needed
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
    int number = 1;
    std::list<question> seqTemp;
    for (int y = locatorMinY; y <= locatorMaxY; y++) {
        for (int x = locatorMinX; x <= locatorMaxX; x++) {
            if (locatorMap.count(x) && locatorMap[x].count(y)) {
                if (locatorMap[x][y].hasLeNumber()) {
                    if (number <= 9) {
                        std::cout << (unsigned char)(number + 48);
                    } else {
                        std::cout << (unsigned char)(number + 72);
                    }
                    debugFile << "NUMBER: " << number << std::endl;

                    std::map<int, Node> &mapXCache = locatorMap[x];

                    seqTemp.emplace_back();
                    question &an = seqTemp.back();
                    an.number = number;
                    Node &n = mapXCache[y];
                    an.body.push_back(&n);

                    const bool isHorizontal = 
                        !(locatorMap.count(x-1) && locatorMap[x-1].count(y)) &&
                        locatorMap.count(x+1) && locatorMap[x+1].count(y)
                    ;
                    const bool isVertical = !mapXCache.count(y-1) && mapXCache.count(y+1);

                    if (isHorizontal) {
                        for (int tx = x+1; locatorMap.count(tx) && locatorMap[tx].count(y); tx++) {
                            an.body.push_back(&locatorMap[tx][y]);
                        }
                        if (isVertical) {
                            seqTemp.emplace_back();
                            question &ana = seqTemp.back();
                            ana.number = -number; // negation indicates vertical
                            ana.body.push_back(&n);
                            for (int ty = y+1; mapXCache.count(ty); ty++) {
                                ana.body.push_back(&locatorMap[x][ty]);
                            }
                        }
                    } else {
                        for (int ty = y+1; mapXCache.count(ty); ty++) {
                            an.body.push_back(&locatorMap[x][ty]);
                        }
                    }
                    number++;
                } else {
                    std::cout << "0";
                }
            } else {
                std::cout << " ";
                // res[x][y] = 0;
            }
        }
        std::cout << std::endl;
    }
    // Insertion sort, by length
    question seq[seqTemp.size()];
    int seqLength = 0;
    for (int length = 1; seqTemp.size() != 0; length++) {
        for (std::list<question>::iterator it = seqTemp.begin(); it != seqTemp.end();) {
            if (it->body.size() == length) {
                seq[seqLength++] = std::move(*it);
                it = seqTemp.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<answer> ans;
    {
        std::list<std::string> temp;
        std::ifstream answers(answerPath);
        std::string buff;
        // A little scheme to avoid superfluous iteratons when sorting: get min length beforehand
        int length = 0xFFFFFF;
        for (char c; !answers.eof();) {
            answers.get(c);
            // Convert to lowercase
            if (c >= 'A' && c <= 'Z') {
                c += 0x20;
            }
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
        // Insertion sort, by length; best, O(n); worst, O(30n); most of the time it's like O(13n) or so
        for (; temp.size() != 0; length++) {
            for (std::list<std::string>::iterator it = temp.begin(); it != temp.end();) {
                if (it->length() == length) {
                    ans.push_back(answer(std::move(*it)));
                    it = temp.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    // Debug
    for (answer &s : ans) {
        std::cout << s.value << std::endl;
    }
    for (int i = 0; i < seqLength; i++) {
        std::cout << "#" << seq[i].number << " is len " << seq[i].body.size() << std::endl;
    }

    // Pre-checks, just to verify correct answer set
    for (int i = 0; i < seqLength; i++) {
        std::vector<Node*> &vec = seq[i].body;
        bool good = false;
        for (answer &s : ans) {
            if (!s.used && s.value.length() == vec.size()) {
                s.used = good = true;
                break;
            }
        }
        if (!good) {
            std::cout << "ERROR: question #" << (i+1) << " without possible answer" << std::endl;
            exit(1);
        }
    }
    for (answer &s : ans) {
        s.used = false;
    }

    Solver sol(ans, seq, seqLength);
    sol.solve();

    for (int y = locatorMinY; y <= locatorMaxY; y++) {
        for (int x = locatorMinX; x <= locatorMaxX; x++) {
            if (locatorMap.count(x) && locatorMap[x].count(y)) {
                std::cout << locatorMap[x][y].val;
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
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

void IParser::parseBox(int i, const int &x, const int &y, const char &direction) {

    // std::cout << "Erm..." << std::endl;
    if (i < size && i >= 0 && addLocation(x, y, i)) {

        // Reposition to account for unpredictable box size fluctuations
        for (; pixelColorEq(i, 0xFF); i += channels);
        i -= channels * 3;
        for (; pixelColorEq(i, 0xFF); i += movVert);
        i -= movVert * 3;

        debug_printCoords(i);
        // Sleep(1000);
        // std::cout << "Hello?" << std::endl;
        debugFile << "Pricess A:" << std::endl;
        debug_printCoords(i - moveHorzSqr);
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
        debugFile << "Pricess C:" << std::endl;
        debug_printCoords(i - moveHorzSqr);
        if (inBox(i - moveHorzSqr)) { // West
            // std::cout << "Sub " << moveHorzSqr << std::endl;
            debugFile << "Allowing WEST with i = " << (i - moveHorzSqr) << std::endl;
            parseBox(i - moveHorzSqr, x-1, y, 2);
        }
        debugFile << "Pricess D:" << std::endl;
        debug_printCoords(i - movVertSqr);
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

bool IParser::addLocation(int x, int y, const int &i) {
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
        std::map<int, Node> leMap;
        leMap.emplace(std::move(y), Node(getCanHorizontal(i)));
        locatorMap.emplace(std::move(x), std::move(leMap));
        totalAns++;
        return true;
    } else {
        std::map<int, Node> &leMap = locatorMap[std::move(x)];
        if (!leMap.count(y)) {
            leMap.emplace(std::move(y), Node(getCanHorizontal(i)));
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

bool IParser::inBox(int index) {
    if (index < 0 || index >= size) {
        debugFile << ">> end Fillet" << std::endl;
        return false;
    }
    // std::cout << "Run" << std::endl;
    // std::cout << "1" << std::endl;
    if (!pixelColorEq(index, 0xFF)) {
        debugFile << ">> end \"Was I wrong?\"" << std::endl;
        debug_printCoords(index);
        return false;
    }

    for (int iter = 0; pixelColorEq(index, 0xFF);) {
        iter++;
        index += channels;
        if (iter > boxDimensions || index > size) {
            debugFile << ">> end WA-" << (iter > boxDimensions) << std::endl;
            return false;
        }
    }
    index -= channels;
    for (int iter = 0; pixelColorEq(index, 0xFF);) {
        iter++;
        index += movVert;
        if (iter > boxDimensions || index > size) {
            debugFile << ">> end WB-" << (iter > boxDimensions) << std::endl;
            return false;
        }
    }
    index -= movVert;

    // std::cout << "2" << std::endl;
    for (int i = index, iter = -2;; i -= movVert) {
        iter++;
        if (iter > boxDimensions || i < 0) {
            debugFile << ">> end B-" << (iter > boxDimensions) << ", " << iter << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            const int hei = std::abs(index - i);
            if (hei > boxDimensionsScaledVertical+movVert*2 || hei < boxDimensionsScaledVertical-movVert*2) {
                debugFile << ">> end C-" << (hei > boxDimensionsScaledVertical) << ", " << ((hei / channels) / width) << std::endl;
                return false;
            } else {
                break;
            }
        }
    }
    // std::cout << "4" << std::endl;
    for (int i = index, iter = -2;; i -= channels) {
        iter++;
        if (iter > boxDimensions || i < 0) {
            debugFile << ">> end F-" << (iter > boxDimensions) << std::endl;
            return false;
        } else if (!pixelColorEq(i, 0xFF)) {
            const int wid = std::abs(index - i);
            if (wid > boxDimensionsScaledHorizontal+channels*2 || wid < boxDimensionsScaledHorizontal-channels*2) {
                debugFile << ">> end G-" << (wid > boxDimensionsScaledHorizontal) << std::endl;
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

char IParser::getCanHorizontal(const int &i) {
    return (char)(
        hasNumber(i) |
        ((inBox(i + moveHorzSqr) && !inBox(i - moveHorzSqr)) << 1) |
        ((inBox(i + movVertSqr) && !inBox(i - movVertSqr)) << 2)
    );
}
