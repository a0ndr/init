#ifndef SINGLETON_H
#define SINGLETON_H

#include <boost/program_options.hpp>

#include "reaper.h"
#include "unit_manager.h"

class Singleton {
    Singleton();
public:
    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

    static Singleton &getInstance();

    UnitManager* unit_manager;

    boost::program_options::variables_map vm = nullptr;
};

#endif //SINGLETON_H
