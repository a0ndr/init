//
// Created by adam on 22. 2. 2025.
//

#ifndef CYCLE_DETECTOR_H
#define CYCLE_DETECTOR_H
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "unit_manager.h"

class CycleDetector {
    std::map<std::string, std::vector<std::shared_ptr<Unit>>> graph;
    std::map<std::string, int> index, lowlink;
    std::set<std::string> onStack;
    std::stack<std::string> stack;
    int currentIndex = 0;
    bool cycleDetected = false;

public:
    explicit CycleDetector();

    void strong_connect(const std::string &node);

    bool detect_cycle();
};


#endif //CYCLE_DETECTOR_H
