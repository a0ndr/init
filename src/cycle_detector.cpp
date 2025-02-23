#include "cycle_detector.h"

#include <iostream>

#include "singleton.h"

CycleDetector::CycleDetector() {
    for (const auto &[name, unit] : Singleton::getInstance().unit_manager->units) {
        graph[name] = {};
        for (const auto &dep : unit->requires_units) graph[name].push_back(dep);
        for (const auto &dep : unit->after_units) graph[name].push_back(dep);
    }
}
void CycleDetector::strong_connect(const std::string &node) {
    index[node] = lowlink[node] = currentIndex++;
    stack.push(node);
    onStack.insert(node);

    for (const auto &neighbor : graph[node]) {
        if (!index.contains(neighbor->name)) {
            strong_connect(neighbor->name);
            lowlink[node] = std::min(lowlink[node], lowlink[neighbor->name]);
        } else if (onStack.contains(neighbor->name)) {
            lowlink[node] = std::min(lowlink[node], index[neighbor->name]);
        }
    }

    if (lowlink[node] == index[node]) {
        std::vector<std::string> scc;
        std::string w;
        do {
            w = stack.top();
            stack.pop();
            onStack.erase(w);
            scc.push_back(w);
        } while (w != node);

        if (scc.size() > 1) {
            cycleDetected = true;
            std::cerr << "Error: Dependency cycle detected involving: ";
            for (const auto &s : scc) std::cerr << s << " ";
            std::cerr << std::endl;
        }
    }
}


bool CycleDetector::detect_cycle() {
    for (const auto &[node, _] : graph) {
        if (!index.contains(node)) {
            strong_connect(node);
        }
    }
    return cycleDetected;
}

