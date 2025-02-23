#ifndef REAPER_H
#define REAPER_H
#include <signal.h>
#include <atomic>

class ProcessReaper {
public:
    ProcessReaper();

    ~ProcessReaper();

private:
    struct sigaction old_action = {};
    static std::atomic<ProcessReaper*> instance;

    void setup_signal_handler();

    static void signal_handler(int signo);
};

#endif //REAPER_H
