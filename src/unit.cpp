#include "unit.h"
#include <boost/algorithm/string/join.hpp>
#include <fmt/format-inl.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "singleton.h"

using namespace std;

void Unit::add_dependency(const shared_ptr<Unit> &svc, const shared_ptr<Unit> &dep) {
    if (ranges::find(Singleton::getInstance().unit_manager->units[svc->name]->before_units,
                     dep) == Singleton::getInstance().unit_manager->units[svc->name]->before_units.end()) {
        Singleton::getInstance().unit_manager->find(svc->name)->before_units.push_back(dep);
        Singleton::getInstance().unit_manager->find(dep->name)->after_units.push_back(svc);
    }
}

void Unit::add_requires(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep) {
    if (ranges::find(Singleton::getInstance().unit_manager->units[svc->name]->requires_units,
                     dep) == Singleton::getInstance().unit_manager->units[svc->name]->requires_units.end()) {
        Singleton::getInstance().unit_manager->find(svc->name)->requires_units.push_back(dep);
        Singleton::getInstance().unit_manager->find(dep->name)->required_by_units.push_back(svc);
    }
}

void Unit::add_wants(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep) {
    if (ranges::find(Singleton::getInstance().unit_manager->units[svc->name]->wants_units,
                     dep) == Singleton::getInstance().unit_manager->units[svc->name]->wants_units.end()) {
        Singleton::getInstance().unit_manager->find(svc->name)->wants_units.push_back(dep);
        Singleton::getInstance().unit_manager->find(dep->name)->wanted_by_units.push_back(svc);
    }
}

void Unit::add_conflict(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep) {
    if (ranges::find(Singleton::getInstance().unit_manager->units[svc->name]->conflicts_units,
                     dep) == Singleton::getInstance().unit_manager->units[svc->name]->conflicts_units.end()) {
        Singleton::getInstance().unit_manager->find(svc->name)->conflicts_units.push_back(dep);
        Singleton::getInstance().unit_manager->find(dep->name)->conflicts_units.push_back(svc);
    }
}

void Unit::resolve_dependencies() {
    for (const auto unit_name: before) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_dependency(std::shared_ptr<Unit>(this), unit);
    }

    for (const auto unit_name: after) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_dependency(unit, std::shared_ptr<Unit>(this));
    }

    for (const auto unit_name: requires_) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_requires(std::shared_ptr<Unit>(this), unit);
    }

    for (const auto unit_name: required_by) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_requires(unit, std::shared_ptr<Unit>(this));
    }

    for (const auto unit_name: wants) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_wants(std::shared_ptr<Unit>(this), unit);
    }

    for (const auto unit_name: wanted_by) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_wants(std::shared_ptr<Unit>(this), unit);
    }

    for (const auto unit_name: conflicts) {
        auto unit = Singleton::getInstance().unit_manager->find(unit_name);
        if (unit == nullptr) {
            unit = create_dummy(unit_name);
        }
        add_conflict(unit, std::shared_ptr<Unit>(this));
    }
}

char** vector_to_execvp(const std::vector<std::string>& args) {
    const auto argv = new char*[args.size() + 1];

    for (size_t i = 0; i < args.size(); i++) {
        argv[i] = new char[args[i].length() + 1];
        strcpy(argv[i], args[i].c_str());
    }

    argv[args.size()] = nullptr;

    return argv;
}

void Unit::start() {
    if (exec_type == ServiceType::SIMPLE) {
        activation = UnitActivation::ACTIVATING;

        if (const pid_t pid = fork(); pid == 0) {
            char** args = vector_to_execvp(exec_start);
            activation = UnitActivation::RUNNING;
            execvp(exec_start[0].c_str(), args);

            for (size_t i = 0; i < exec_start.size(); i++) {
                delete[] args[i];
            }
            delete[] args;
            _exit(1);
        } else {
            this->pid = pid;
        }
    }
}

void Unit::stop(const UnitActivation new_state = UnitActivation::EXITED) {
    if (activation == UnitActivation::RUNNING || activation == UnitActivation::ACTIVATING) return;

    activation = UnitActivation::DEACTIVATING;
    // TODO: stop the service
    activation = new_state;

    for (const auto &dependent : required_by_units) {
        dependent->stop(UnitActivation::FAILED);
    }
}

void Unit::stringify() {
    fmt::print(
        "{}\nDescription: {}\nType: {}\nLoad: {}\nActive: {}\n\nBefore: {}\nAfter: {}\n\nExec Type: {}\nExec Start: {}\nPID: {}\n\n",
        name, description,
        to_string(type), to_string(load), to_string(activation),
        boost::algorithm::join(before, " "),
        boost::algorithm::join(after, " "),
        to_string(exec_type), boost::algorithm::join(exec_start, " "),
        pid);
}

std::shared_ptr<Unit> Unit::create_dummy(const std::string &name) {
    auto unit = std::make_shared<Unit>();
    unit->name = name;
    unit->load = UnitLoad::NOT_FOUND;
    Singleton::getInstance().unit_manager->units[name] = unit;
    return unit;
}
