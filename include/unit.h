#ifndef UNIT_H
#define UNIT_H

#include <memory>
#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <signal.h>

enum class UnitType {
    NONE,
    TARGET,
    SERVICE,
    MOUNT,
    SOCKET,
};

constexpr const char *to_string(const UnitType type) {
    switch (type) {
        case UnitType::NONE: return "none";
        case UnitType::TARGET: return "target";
        case UnitType::SERVICE: return "service";
        case UnitType::MOUNT: return "mount";
        case UnitType::SOCKET: return "socket";
        default: return "unknown";
    }
}

enum class UnitLoad {
    LOADED,
    NOT_FOUND,
    BAD_SETTING,
    MASKED,
};

constexpr const char *to_string(const UnitLoad load) {
    switch (load) {
        case UnitLoad::LOADED: return "loaded";
        case UnitLoad::NOT_FOUND: return "not-found";
        case UnitLoad::BAD_SETTING: return "bad-setting";
        case UnitLoad::MASKED: return "masked";
        default: return "unknown";
    }
}

enum class UnitActivation {
    RUNNING,
    EXITED,
    WAITING,
    INACTIVE,
    FAILED,
    ACTIVATING,
    DEACTIVATING,
};

constexpr const char *to_string(const UnitActivation act) {
    switch (act) {
        case UnitActivation::RUNNING: return "active (running)";
        case UnitActivation::EXITED: return "active (exited)";
        case UnitActivation::WAITING: return "active (waiting)";
        case UnitActivation::INACTIVE: return "inactive";
        case UnitActivation::FAILED: return "failed";
        case UnitActivation::ACTIVATING: return "activating";
        case UnitActivation::DEACTIVATING: return "deactivating";
        default: return "unknown";
    }
}

enum class ServiceType {
    SIMPLE,
    ONESHOT,
    NOTIFY,
};

constexpr const char *to_string(const ServiceType type) {
    switch (type) {
        case ServiceType::SIMPLE: return "simple";
        case ServiceType::ONESHOT: return "oneshot";
        case ServiceType::NOTIFY: return "notify";
        default: return "unknown";
    }
}

class Unit {
public:
    UnitType type = UnitType::NONE;

    // [Unit] keys
    std::string name;
    std::string description;

    std::vector<std::string> requires_;
    std::vector<std::shared_ptr<Unit> > requires_units; // units that are required to start

    std::vector<std::string> wants;
    std::vector<std::shared_ptr<Unit> > wants_units; // units that are optional to start

    std::vector<std::string> conflicts;
    std::vector<std::shared_ptr<Unit> > conflicts_units; // units to be stopped before starting

    std::vector<std::string> before;
    std::vector<std::shared_ptr<Unit> > before_units; // to be started before these units start

    std::vector<std::string> after;
    std::vector<std::shared_ptr<Unit> > after_units; // to be started after these units start

    std::string on_failure;
    std::shared_ptr<Unit> on_failure_unit; // start unit if this unit fails

    // [Install] keys
    std::vector<std::string> wanted_by;
    std::vector<std::shared_ptr<Unit> > wanted_by_units; // defines which target should activate this one

    std::vector<std::string> required_by;
    // similar to wanted_by_units, but enforces strict dependency (failing this unit will fail the parent)
    std::vector<std::shared_ptr<Unit> > required_by_units;

    // [Service] keys
    ServiceType exec_type = ServiceType::SIMPLE;
    std::vector<std::string> exec_start;
    bool remain_after_exit = false; // ServiceType::ONESHOT only
    int kill_signal = SIGTERM;

    UnitLoad load = UnitLoad::LOADED;
    UnitActivation activation = UnitActivation::INACTIVE;
    pid_t pid;

    bool is_running();

    void start();

    void stop(UnitActivation new_state);

    static void add_dependency(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep);

    static void add_requires(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep);

    static void add_wants(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep);

    static void add_conflict(const std::shared_ptr<Unit> &svc, const std::shared_ptr<Unit> &dep);

    void resolve_dependencies();

    void stringify();

    static std::shared_ptr<Unit> create_dummy(const std::string &name);
};

#endif
