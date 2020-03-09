#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

// DEBUG
#include <fstream>
extern std::ofstream debugFile;

// When the program starts, will choose a starting question and test inputs.
// Because of the nature of crossowords, there can be one sole q/a pair.
// This is what START_SEARCH_UNIQUE toggles: the searching for a unique pair to kick off the solving process.
// From testing this seems to affect roughly half of the crosswords on sciencemusicvideos.
// This is toggleable because it could cause problems in the future, and only gives only a small performance boost.
// Perhaps a useless safeguard, however I feel a sense of safety in having it conditional.
#define START_SEARCH_UNIQUE

#endif
