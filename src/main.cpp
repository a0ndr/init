#include <iostream>
#include "singleton.h"
#include <fmt/core.h>
#include <sys/prctl.h>

#include "dag.h"
#include "reaper.h"

using namespace std;
namespace po = boost::program_options;

int main(const int argc, char *argv[]) {
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

    const Singleton &instance = Singleton::getInstance();
    try {
        instance.unit_manager->load_units();
    } catch (char *e) {
        cerr << "Failed to load units: " << e << endl;
        return 1;
    }

    instance.unit_manager->resolve_dependencies();

    if (const auto t = instance.unit_manager->units.find(default_target); t != nullptr) {
        instance.unit_manager->default_target = t->second;
    } else {
        cerr << fmt::format("Default target \"{}\" not found", default_target) << endl;
        return 1;
    }

    // for (auto [_, unit] : instance.unit_manager->units) {
        // unit->stringify();
    // }

    DAG dag{};
    dag.build_graph();
    dag.build_queue();

    // fmt::print("\nService start-up order:\n");
    // for (auto unit: dag.order) {
    //     fmt::println("{}", unit->name);
    // }

    // for (auto [fst, snd]: instance.unit_manager->default_target) {
    //     fmt::print("{} {}\n", fst, snd->description);
    // }
}
