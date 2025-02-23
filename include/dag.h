//
// Created by adam on 22. 2. 2025.
//

#ifndef DAG_H
#define DAG_H
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "unit.h"

class DAG {
public:
    std::map<std::string, int> indegree;
    std::map<std::string, std::vector<std::shared_ptr<Unit>>> adj;
    std::vector<std::shared_ptr<Unit>> order;
    std::queue<std::shared_ptr<Unit>> q;
    std::set<std::shared_ptr<Unit>> processed;

    void build_graph();

    void build_queue();
};

#endif //DAG_H
