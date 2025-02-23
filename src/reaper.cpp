#include "reaper.h"

#include <iostream>
#include <system_error>
#include <sys/wait.h>

ProcessReaper::ProcessReaper() {
    setup_signal_handler();
}

ProcessReaper::~ProcessReaper() {
    if (old_action.sa_handler != nullptr) {
        sigaction(SIGCHLD, &old_action, nullptr);
    }
}

void ProcessReaper::setup_signal_handler() {
    struct sigaction action = {};
    action.sa_handler = signal_handler;
    action.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    sigemptyset(&action.sa_mask);

    if (sigaction(SIGCHLD, &action, &old_action) == -1) {
        throw std::system_error(errno, std::generic_category(), "sigaction failed");
    }

    instance.store(this);
}

void ProcessReaper::signal_handler(int signo) {
    if (auto* reaper = instance.load()) {
        pid_t pid;
        int status;

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED(status)) {
                std::cout << "Process " << pid << " exited with code "
                        << WEXITSTATUS(status) << std::endl;
            } else if (WIFSIGNALED(status)) {
                std::cout << "Process " << pid << " terminated by signal "
                        << WTERMSIG(status) << std::endl;
            }
        }
    }
}
