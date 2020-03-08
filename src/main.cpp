#include <iostream>

#include "IParser.h"
#include "Solver.h"

#include "main.h"

// DEBUG
#include <fstream>
std::ofstream debugFile("debug.log");

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Invalid # of arguments, must be `<imagefile> <answerdoc>`" << std::endl;
        return 1;
    }
    IParser parse(argv[1], argv[2]);
    return 0;
}
