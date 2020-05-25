#include <iostream>

#include "../external/stb_image.h"
#include "IParser.h"
#include "Solver.h"

#include "main.h"

// DEBUG
#include <fstream>
std::ofstream debugFile("crossot.log");

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid # of arguments, must be `<imagefile> <answerdoc>`" << std::endl;
        return 1;
    }
    int width;
    int height;
    int channels;
    imagebpm image = stbi_load(argv[1], &width, &height, &channels, 0);
    if (image == NULL) {
        std::cerr << "Error: Failed to load image: " << stbi_failure_reason() << std::endl;
        exit(1);
    }
    IParser parse(width, height, channels, image);
    parse.run(argv[2]);
    return 0;
}
