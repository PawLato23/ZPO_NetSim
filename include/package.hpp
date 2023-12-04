#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

#include <set>

using ElementID = std::size_t;

class Package{
public:
    Package();          // @NOT IMPLEMENTED
    Package(ElementID Element_ID): _ID(Element_ID) {};
    Package(Package&& pack);    // @NOT IMPLEMENTED
    ~Package();             // @NOT IMPLEMENTED

    ElementID get_id() const {return _ID;}

    operator=(Package&& pack); // @NOT IMPLEMENTED
private:
    ElementID _ID;
    static std::set<ElementID> _assigned_IDs;
    static std::set<ElementID> _freed_IDs;
};

#endif //NETSIM_PACKAGE_HPP
