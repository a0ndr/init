#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "defines.h"

inline int get_signal(const std::string& signalName) {
    static const std::unordered_map<std::string, int> signalMap = {
        {"SIGHUP", SIGHUP},
        {"SIGINT", SIGINT},
        {"SIGQUIT", SIGQUIT},
        {"SIGILL", SIGILL},
        {"SIGTRAP", SIGTRAP},
        {"SIGABRT", SIGABRT},
        {"SIGBUS", SIGBUS},
        {"SIGFPE", SIGFPE},
        {"SIGKILL", SIGKILL},
        {"SIGUSR1", SIGUSR1},
        {"SIGSEGV", SIGSEGV},
        {"SIGUSR2", SIGUSR2},
        {"SIGPIPE", SIGPIPE},
        {"SIGALRM", SIGALRM},
        {"SIGTERM", SIGTERM},
        {"SIGCHLD", SIGCHLD},
        {"SIGCONT", SIGCONT},
        {"SIGSTOP", SIGSTOP},
        {"SIGTSTP", SIGTSTP},
        {"SIGTTIN", SIGTTIN},
        {"SIGTTOU", SIGTTOU},
        {"SIGXCPU", SIGXCPU},
        {"SIGXFSZ", SIGXFSZ},
        {"SIGVTALRM", SIGVTALRM},
        {"SIGPROF", SIGPROF},
        {"SIGWINCH", SIGWINCH},
        {"SIGIO", SIGIO},
        {"SIGPWR", SIGPWR},
        {"SIGSYS", SIGSYS}
    };

    auto it = signalMap.find(signalName);
    if (it != signalMap.end()) {
        return it->second;
    }
    return -1; // Return -1 if the signal name is not found
}

namespace fs = std::filesystem;
namespace bfs = boost::filesystem;

inline std::string get_filename(const std::string &full_path) {
    if (const size_t pos = full_path.find_last_of('/'); pos != std::string::npos) {
        return full_path.substr(pos + 1);
    }
    return full_path;
}

inline std::string get_ext(const std::string &file_name) {
    if (const size_t pos = file_name.find_last_of('.'); pos != std::string::npos) {
        return file_name.substr(pos + 1);
    }
    return file_name;
}

inline std::string get_path(const std::string &file_name) {
    const fs::path fs_file_path = fs::path(INIT_UNITS_PATH) / fs::path(file_name);
    return fs_file_path.string();
}

inline std::vector<std::string> split(const char* s, const char *delim) {
    std::vector<std::string> elems;
    if (s == nullptr) return elems;

    boost::split(elems, s, boost::is_any_of(delim));
    return elems;
}

inline bool word2bool(const char* i) {
    auto x = const_cast<char*>(i);
    boost::to_lower(x);
    return x == "yes" || x == "true" || x == "1" || x == "y";
}

#endif //UTIL_H
