#include "package.hpp"

Package::~Package() {
    if (_freed_IDs.size() == 0)
        _freed_IDs = {this->get_id()};
    else {
        _freed_IDs.insert(this->get_id());

    }
}
Package::Package() {
    Package(5);
}