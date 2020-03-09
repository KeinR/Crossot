#ifndef SOLVE_H_INCLUDED
#define SOLVE_H_INCLUDED

#include <string>
#include <vector>

#include "Node.h"
#include "main.h"

struct answer {
    answer();
    answer(std::string &&value);
    std::string value;
    bool used = false;
};

struct question {
    std::vector<Node*> body;
    int number;
    bool used = false;
};

class Solver {
    public:
    Solver(std::vector<answer> &ans, question *seq, const int &seqLength);
    void solve();
    private:
    struct change {
        int seqIndex;
        int ansIndex;
    };
    std::vector<change> changeStack;
    std::vector<answer> &ans;
    question *&seq;
    const int &seqLength;
    bool rec(int id);
    #ifdef START_SEARCH_UNIQUE
    void tryUnique();
    #endif // end START_SEARCH_UNIQUE
};

#endif
