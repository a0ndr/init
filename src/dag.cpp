#include "dag.h"

#include "cycle_detector.h"
#include "singleton.h"

void DAG::build_graph() {
    for (auto &[name, unit]: Singleton::getInstance().unit_manager->units) {
        indegree[name] = unit->requires_units.size() + unit->after_units.size();

        for (const auto &dep: unit->requires_units) adj[dep->name].push_back(unit);
        for (const auto &dep: unit->after_units) adj[dep->name].push_back(unit);
    }
}

void DAG::build_queue() {
    if (CycleDetector detector{}; detector.detect_cycle()) return;

    for (auto &[name, count]: indegree) {
        if (count == 0) q.push(Singleton::getInstance().unit_manager->find(name));
    }

    while (!q.empty()) {
        std::shared_ptr<Unit> service = q.front();
        q.pop();
        order.push_back(service);
        processed.insert(service);

        // Process Requires= and After=
        for (const auto &dep: adj[service->name]) {
            if (--indegree[dep->name] == 0) {
                q.push(dep);
            }
        }

        // Start Wants= services (if they exist)
        for (const auto &wantedService: service->wants_units) {
            order.push_back(wantedService);
            processed.insert(wantedService);
        }
    }
}
