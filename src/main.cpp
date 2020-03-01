#include <iostream>
#include <list>
#include <iomanip>
#include <chrono>

#include "main.h"
#include "../external/stb_image.h"

int main(int argc, char *argv[]) {
    int width, height, channels;
    const unsigned char *const img = stbi_load(argv[1], &width, &height, &channels, 0);
    if (img == NULL) {
        std::cout << "Error: Failed to load image" << std::endl;
        return 1;
    }
    if (width < 10 || height < 10) {
        std::cout << "Error: handle size too small" << std::endl;
        // return 1;
    }

    const Handle handle = {
        img,
        width,
        height,
        channels,
        channels-1,
        width * channels,
        handle.movVert * 16,
        width * height * channels,
        img[0],
        img[1],
        img[2]
    };

    std::cout << handle.movVert << std::endl;
    std::cout << handle.movVertSqr << std::endl;

    // Ignores alpha

    std::cout << "Width == " << width << std::endl;
    std::cout << "Height == " << height << std::endl;
    std::cout << "Size == " << handle.size << std::endl;

    std::cout << "Channles " << handle.channels << std::endl;

    std::cout << "BG = " << (int)handle.img[0] << ", " << (int)handle.img[1] << ", " << (int)handle.img[2] << std::endl;

    int i = handle.channels;
    // Find first box and determine box size
    for (; i+handle.channelsM1 < handle.size; i += handle.channels) {
        if (handle.img[i] != handle.r || handle.img[i+1] != handle.b || handle.img[i+2] != handle.g) {
            // std::cout << "Yes" << std::endl;
            // std::cout << (i / channels) << std::endl;
            std::cout << "+++" << std::endl;
            std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
            i += 10 * handle.channels;
            i += handle.movVert * 8;
            std::cout << ((i / channels) % width) << ", " << ((i / channels) / width) << std::endl;
            std::cout << "+++" << std::endl;
            break;
        }
    }
    parseSect(handle, i, true);
    // bool vertical;
    // int iu = i + handle.movVertSqr;
    // if (!pixelColorEq(handle, iu)) {
    //     parseSect(handle, iu, true);
    // }
    // iu = i + BOX_WIDTH;
    // if (!pixelColorEq(handle, iu)) {
    //     parseSect(handle, iu, false);
    // }
    std::cout << "Sequencing complete(?)" << std::endl;
}

void parseSect(const Handle &handle, int i, bool vertical) {
    const int start = i;
    parseNumber(handle, i);
    // if (vertical) {
    //     std::list<node> list;
    //     for (;; i += handle.movVertSqr) {
    //         if () {

    //         }
    //     }
    // } else {

    // }
}

bool Data::addEntree(const Handle &handle, const int &i, const node &n) {
    if (!data.count(i)) {
        data[i] = n;
        return true;
    }
    return false;
}

int parseNumber(const Handle &handle, int i) {
    int map[BOX_DIMENSIONS][BOX_DIMENSIONS];
    std::cout << "------------------------------------------" << std::endl;
    std::cout << ((i / handle.channels) % handle.width) << ", " << ((i / handle.channels) / handle.width) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    for (;; i -= handle.movVert) {
        if (!pixelColorEq(handle, i, 0xFF)) {
            i += handle.movVert;
            break;
        }
    }
    for (;; i += handle.channels) {
        if (!pixelColorEq(handle, i, 0xFF)) {
            i -= handle.channels;
            break;
        }
    }
    std::cout << "------------------------------------------" << std::endl;
    std::cout << ((i / handle.channels) % handle.width) << ", " << ((i / handle.channels) / handle.width) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    for (int c = 0, r = 0;;c++) {
        if (c >= BOX_DIMENSIONS) {
            c = 0;
            r++;
            if (r >= BOX_DIMENSIONS) {
                break;
            }
        }
        const int loc = i - c * handle.channels + r * handle.movVert;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~";
        std::cout << c << std::endl;
        std::cout << (loc / handle.channels) << std::endl;
        std::cout << ((loc / handle.channels) % handle.width) << ", " << ((loc / handle.channels) / handle.width) << std::endl;
        map[c][r] = (handle.img[loc] << 16) | (handle.img[loc+1] << 8) | handle.img[loc+2];
    }

    std::cout << std::hex;

    for (int y = 0; y < BOX_DIMENSIONS; y++) {
        for (int x = 0; x < BOX_DIMENSIONS; x++) {
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
    return 1;
}

bool pixelColorEq(const Handle &handle, const int &index) {
    return handle.r == handle.img[index] && handle.b == handle.img[index+1] && handle.g == handle.img[index+2];
}

bool pixelColorEq(const Handle &handle, const int &index, const int val) {
    return val == handle.img[index] && val == handle.img[index+1] && val == handle.img[index+2];
}
