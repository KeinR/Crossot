#include <iostream>
#include <string>
#include <vector>

#include "Solver.h"
#include "main.h"

answer::answer() {
}
answer::answer(std::string &&v): value{std::move(v)} {
}

Solver::Solver(std::vector<answer> &ans, question *seq, const int &seqLength):
ans(ans), seq(seq), seqLength(seqLength) {
}

void Solver::solve() {
    if (!rec(0)) {
        std::cout << "Can't do it I'm afraid..." << std::endl;
        exit(1);
    }
}

bool Solver::rec(int id) {
    id++;
    // Find if there's a unique
    // const int index = getIndexUniqueAns();
    // std::cout << "Index unique: " << index << std::endl;
    if (/*index == -1 || */true) {

        int indexMost = -1;
        for (int mostMatch = -1, i = 0; i < seqLength; i++) {
            question &ques = seq[i];
            if (!ques.used) {
                int personalTotal = 0;
                for (int f = 0; f < ques.body.size(); f++) {
                    if (ques.body[f]->changeId != 0) {
                        personalTotal++;
                    }
                }
                if (personalTotal > mostMatch) {
                    mostMatch = personalTotal;
                    indexMost = i;
                }
            }
        }

        if (indexMost == -1) {
            debugFile << "True" << std::endl;
            return true;
        }

        question &ques = seq[indexMost];
        ques.used = true;
        debugFile << "Select " << ques.number << std::endl;
        const int targetLength = ques.body.size();
        for (int i = 0; i < ans.size(); i++) {
            if (!ans[i].used && ans[i].value.length() == targetLength) {
                // He he look i reuse variable i i'm so clever
                for (; i < ans.size() && targetLength == ans[i].value.length(); i++) {
                    if (ans[i].used) {
                        continue;
                    }
                    std::string &val = ans[i].value;
                    bool match = true;
                    for (int c = 0; c < targetLength; c++) {
                        if (ques.body[c]->changeId != 0 && val[c] != ques.body[c]->val) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        ans[i].used = true;
                        for (int c = 0; c < targetLength; c++) {
                            Node *&n = ques.body[c];
                            if (n->changeId == 0) {
                                n->val = val[c];
                                n->changeId = id;
                            }
                        }
                        debugFile << "#" << ques.number << " as " << val << std::endl;
                        if (rec(id)) {
                            debugFile << "Last done" << std::endl;
                            return true;
                        }
                        // Didn't work :(
                        // Rollback changes and try again
                        for (int c = 0; c < targetLength; c++) {
                            Node *&n = ques.body[c];
                            if (n->changeId == id) {
                                n->changeId = 0;
                                n->val = n->changeId + 0x30;
                            }
                        }
                        ans[i].used = false;
                    }
                }
                ques.used = false;
                debugFile << "--return--" << std::endl;
                return false; // The last recursion messed up, better let them know...
            }
        }

        // Welp
        debugFile << "CRITICAL ERROR: Answers depleated, yet all questions not filled" << std::endl;
        exit(1);

    } else {
        // answer &a = ans[index];
        // a.used = true;
        // bool done = true;
        // for (int i = 0; i < seqLength; i++) {
        //     question &q = *(seq+i);
        //     if (q.used) {
        //         done = false;
        //     } else if (q.body.size() == a.value.length()) {
        //         for (int i = 0; i < q.body.size(); i++) {
        //             q.body[i]->val = a.value[i];
        //             q.body[i]->changeId = id;
        //         }
        //         q.used = true;
        //         break;
        //     }
        // }
        // return done ? true : rec(id);
    }
}

int Solver::getIndexUniqueAns() {
    for (int i = 0; i < ans.size();) {
        const int length = ans[i].value.length();
        const int li = i;
        bool good = true;
        while (++i < ans.size() && length == ans[i].value.length()) {
            if (!ans[i].used) {
                good = false;
            }
        }
        if (good) {
            return li;
        }
    }
    return -1;
}
