#include "singleton.h"

#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <SimpleIni.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/base.h>

#include "defines.h"

namespace fs = std::filesystem;
namespace bfs = boost::filesystem;

Singleton &Singleton::getInstance() {
    static Singleton instance;
    return instance;
}

Singleton::Singleton() {
}

std::vector<std::shared_ptr<Unit>> split_n_load(const CSimpleIniA &ini, const std::string &section,
                                 const std::string &key) {
    std::vector<std::shared_ptr<Unit>> units;

    auto value = ini.GetValue(section.c_str(), key.c_str());
    if (value == nullptr) {
        return units;
    }

    std::vector<std::string> result;
    split(result, value, boost::is_any_of(" "));

    for (auto unit: result) {
        bfs::path path = bfs::path(INIT_UNITS_PATH) / bfs::path(unit);
        units.push_back(Singleton::getInstance().load_unit(path.c_str()));
    }

    return units;
}

std::string get_filename(const std::string &full_path) {
    if (const size_t pos = full_path.find_last_of("/"); pos != std::string::npos) {
        return full_path.substr(pos + 1);
    }
    return full_path;
}

std::string get_path(const std::string &file_name) {
    const bfs::path bfs_file_path = INIT_UNITS_PATH / bfs::path(file_name);
    return bfs::path(bfs_file_path).string();
}

std::shared_ptr<Unit> Singleton::load_unit(const std::string &file_name) {
    const bfs::path bfs_file_path = INIT_UNITS_PATH / bfs::path(file_name);
    auto file_path = bfs_file_path.c_str();

    if (fs::is_symlink(file_path)) {
        file_path = realpath(file_path, nullptr);
    }

    fmt::println("{}", file_path);

    CSimpleIniA ini;
    // ini.SetUnicode();

    if (const SI_Error rc = ini.LoadFile(file_path); rc < 0) {
        throw strerror(errno);
    }

    const auto unit = std::make_shared<Unit>();
    unit->name = file_name;

    unit->description = ini.GetValue("Unit", "Description");

    unit->requires_units = split_n_load(ini, "Unit", "Requires");
    unit->wants_units = split_n_load(ini, "Unit", "Wants");
    unit->conflicts_units = split_n_load(ini, "Unit", "Conflicts");
    unit->before_units = split_n_load(ini, "Unit", "Before");
    unit->after_units = split_n_load(ini, "Unit", "After");
    if (const auto on_failure = ini.GetValue("Unit", "OnFailure"); on_failure != nullptr) {
        unit->on_failure_unit = load_unit(on_failure);
    }

    unit->wanted_by_units = split_n_load(ini, "Unit", "WantedBy");
    unit->required_by_units = split_n_load(ini, "Unit", "RequiredBy");

    return unit;
}


void Singleton::load_units() {
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
                // units.insert_or_assign(get_filename(realpath(get_path(file_name).c_str(), nullptr)),
                //                        load_unit(file_name));
                units.insert_or_assign(file_name, load_unit(file_name));
            } catch (...) {
                throw;
            }
        }
    }

    closedir(dir);
}
