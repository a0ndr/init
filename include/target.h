#ifndef TARGET_H
#define TARGET_H

#include <iostream>

class Target {
  public:
    Target(std::string name, std::string description);
    std::string name;
    std::string description;
};

#endif
