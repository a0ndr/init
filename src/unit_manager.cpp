#include <unit_manager.h>

#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <optional>
#include <SimpleIni.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/base.h>

#include "defines.h"
#include "singleton.h"
#include "util.h"

namespace fs = std::filesystem;

std::shared_ptr<Unit> UnitManager::load_unit(const std::string &file_name) {
    const auto unit = std::make_shared<Unit>();
    unit->name = file_name;

    if (const auto ext = get_ext(file_name); ext == "target") {
        unit->type = UnitType::TARGET;
    } else if (ext == "service") {
        unit->type = UnitType::SERVICE;
    } else {
        unit->type = UnitType::NONE;
        unit->load = UnitLoad::BAD_SETTING;
    }

    // if (fs::is_symlink(file_path)) {
    //     file_path = realpath(file_path, nullptr);
    // }

    CSimpleIniA ini;
    ini.SetUnicode();

    if (const SI_Error rc = ini.LoadFile(get_path(file_name).c_str()); rc < 0) {
        throw strerror(errno);
    }

    unit->description = ini.GetValue("Unit", "Description") != nullptr ? ini.GetValue("Unit", "Description") : "";

    unit->requires_ = split(ini.GetValue("Unit", "Requires"), " ");
    unit->wants = split(ini.GetValue("Unit", "Wants"), " ");
    unit->conflicts = split(ini.GetValue("Unit", "Conflicts"), " ");
    unit->before = split(ini.GetValue("Unit", "Before"), " ");
    unit->after = split(ini.GetValue("Unit", "After"), " ");
    unit->on_failure = ini.GetValue("Unit", "Before") != nullptr ? ini.GetValue("Unit", "Before") : "";

    unit->wanted_by = split(ini.GetValue("Install", "WantedBy"), " ");
    unit->required_by = split(ini.GetValue("Install", "RequiredBy"), " ");

    if (unit->type == UnitType::SERVICE) {
        unit->exec_start = split(ini.GetValue("Service", "ExecStart"), " ");

        if (const auto type = ini.GetValue("Service", "Type", ""); type == "simple") {
            unit->exec_type = ServiceType::SIMPLE;
        } else if (type == "oneshot") {
            unit->exec_type = ServiceType::ONESHOT;
        } else if (type == "notify") {
            unit->exec_type = ServiceType::NOTIFY;
        } else {
            unit->exec_type = ServiceType::SIMPLE;
        }

        unit->remain_after_exit = word2bool(ini.GetValue("Service", "RemainAfterExit", "no"));
        unit->kill_signal = get_signal(ini.GetValue("Service", "KillSignal", "SIGTERM"));
    }

    return unit;
}


void UnitManager::load_units() {
    units.clear();

    DIR *dir = opendir(INIT_UNITS_PATH);

    if (dir == nullptr) {
        throw strerror(errno);
    }

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (const auto file_name = entry->d_name;
            boost::algorithm::ends_with(file_name, ".target")
            || boost::algorithm::ends_with(file_name, ".service")) {
            try {
                units.insert_or_assign(file_name, load_unit(file_name));
            } catch (char *) {
                throw;
            }
        }
    }

    closedir(dir);
}

std::shared_ptr<Unit> UnitManager::find(const std::string &name) {
    if (const auto unit = units.find(name); unit != nullptr) {
        return unit->second;
    }
    return nullptr;
}


void UnitManager::resolve_dependencies() {
    for (auto [_, unit]: units) {
        unit->resolve_dependencies();
        // unit->stringify();
    }
}
