#include "singleton.h"

#include <iostream>

Singleton &Singleton::getInstance() {
    static Singleton instance;
    return instance;
}

std::atomic<ProcessReaper *> ProcessReaper::instance{nullptr};

Singleton::Singleton() {
    unit_manager = new UnitManager();

    try {
        ProcessReaper reaper;

        if (const pid_t pid = fork(); pid == 0) {
            if (const pid_t child_pid = fork(); child_pid == 0) {
                sleep(1);
                exit(1);
            }
            sleep(0.5);
            exit(0);
        }

        while (true) {
            sleep(1);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
