#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <memory>
#include <vector>

class Unit {
public:
    // [Unit] Keys
    std::string name;
    std::string description;

    std::vector<std::shared_ptr<Unit>> requires_units; // units that are required to start
    std::vector<std::shared_ptr<Unit>> wants_units; // units that are optional to start
    std::vector<std::shared_ptr<Unit>> conflicts_units; // units to be stopped before starting
    std::vector<std::shared_ptr<Unit>> before_units; // to be started before these units start
    std::vector<std::shared_ptr<Unit>> after_units; // to be started after these units start
    std::shared_ptr<Unit> on_failure_unit; // start unit if this unit fails

    std::vector<std::shared_ptr<Unit>> wanted_by_units; // defines which target should activate this one
    std::vector<std::shared_ptr<Unit>> required_by_units; // similar to wanted_by_units, but enforces strict dependency (failing this unit will fail the parent)
};

#endif
