#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "unit.h"

class UnitManager {
public:
    std::unordered_map<std::string, std::shared_ptr<Unit> > units;
    std::shared_ptr<Unit> default_target = nullptr;

    static std::shared_ptr<Unit> load_unit(const std::string &file_name);

    std::shared_ptr<Unit> find(const std::string &name);

    void load_units();
    void resolve_dependencies();
};

#endif //UNIT_MANAGER_H
