#include <iostream>
#include "singleton.h"
#include <fmt/core.h>

using namespace std;
namespace po = boost::program_options;

int main(const int argc, char* argv[]) {
    string default_target;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Print this help message")
            ("default-target", po::value<std::string>(&default_target)->default_value("default.target"),
             "The default target when booting up");

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.contains("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    Singleton &instance = Singleton::getInstance();
    try {
        instance.load_units();
    } catch (char *e) {
        cerr << "Failed to load units: " << e << endl;
        return 1;
    }

    if (const auto t = instance.units.find(default_target); t != nullptr) {
        instance.default_target = t->second;
    } else {
        cerr << fmt::format("Default target \"{}\" not found", default_target) << endl;
        return 1;
    }

    for (auto unit: instance.units) {
        fmt::print("{} {}\n", unit.first, unit.second->description);
    }
}
