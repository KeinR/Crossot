#include <iostream>
#include <string>
#include <vector>

#include "Solver.h"

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
    }
}

bool Solver::rec(int id) {
    id++;
    // Find if there's a unique
    const int index = getIndexUniqueAns();
    std::cout << "Index unique: " << index << std::endl;
    if (index == -1 || true) {

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
            return true;
        }

        question &ques = seq[indexMost];
        ques.used = true;
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
                        for (int c = 0; c < targetLength; c++) {
                            Node *&n = ques.body[c];
                            if (n->changeId == 0) {
                                n->val = val[c];
                                n->changeId = id;
                            }
                        }
                        if (rec(id)) {
                            return true;
                        }
                        // Didn't work :(
                        // Rollback changes and try again
                        for (int c = 0; c < targetLength; c++) {
                            Node *&n = ques.body[c];
                            if (n->changeId == id) {
                                n->changeId = 0;
                            }
                        }
                        ques.used = false;
                    }
                }
                ques.used = false;
                return false; // The last recursion messed up, better let them know...
            }
        }

        // Welp
        std::cout << "CRITICAL ERROR: Answers depleated, yet all questions not filled" << std::endl;
        exit(1);

    } else {
        answer &a = ans[index];
        a.used = true;
        bool done = true;
        for (int i = 0; i < seqLength; i++) {
            question &q = *(seq+i);
            if (q.used) {
                done = false;
            } else if (q.body.size() == a.value.length()) {
                for (int i = 0; i < q.body.size(); i++) {
                    q.body[i]->val = a.value[i];
                    q.body[i]->changeId = id;
                }
                q.used = true;
                break;
            }
        }
        return done ? true : rec(id);
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
