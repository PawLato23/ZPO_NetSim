#include "package.hpp"

Package::~Package() {
    if (_freed_IDs.size() == 0)
        _freed_IDs = {_ID};
    else {
        _freed_IDs.insert(_ID);
    }
    _assigned_IDs.erase(_assigned_IDs.find(_ID));
}
Package::Package() {
    if(_freed_IDs.size() > 0) {
        Package(*_freed_IDs.cbegin());
        _freed_IDs.erase(_freed_IDs.begin());
    } else if(_assigned_IDs.size() == 0)
        Package(1);
    else{
        Package(*(_assigned_IDs.cend())++);
    }
}

Package::Package(ElementID Element_ID): _ID(Element_ID){
    //if(_assigned_IDs.count(Element_ID) > 0)
    //    throw std::runtime_error("not unique ID for new product");

    _assigned_IDs.insert(Element_ID);
}