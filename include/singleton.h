#ifndef SINGLETON_H
#define SINGLETON_H

#include <unordered_map>
#include <boost/program_options.hpp>
#include "unit.h"
#include <memory>

class Singleton {
    Singleton();

    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

public:
    static Singleton &getInstance();

    boost::program_options::variables_map vm = nullptr;

    std::unordered_map<std::string, std::shared_ptr<Unit>> units;
    std::shared_ptr<Unit> default_target = nullptr;

    static std::shared_ptr<Unit> load_unit(const std::string &file_name);
    void load_units();
};

#endif //SINGLETON_H
