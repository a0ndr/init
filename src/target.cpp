#include "target.h"
#include <iostream>
#include <utility>

using namespace std;

Target::Target(string name, string description) {
    this->name = std::move(name);
    this->description = std::move(description);
}
