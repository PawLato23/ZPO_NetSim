#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

#include <set>
#include <ostream>

using ElementID = std::size_t;

/*
 * T&& - tzw. rvalue reference - nie tworzy kopii, a przypisuje tymczasowo obiekt
 * Po co są i jak mają działać Package(&&) i operator=. Najpewniej operator= będzie agregował Package(&&)
 */

class Package{
public:
    Package();          // @TO TEST //tworzy nowy produkt z unikatowym ID
    Package(ElementID Element_ID); //pytanie, czy to ma mieć zakaz tworzenia elementu o tym samym ID //tworzy produkt o zadanym nieunikatowym ID\
    Package(Package&& prod);    // @NOT IMPLEMENTED //??
    ~Package();             // @TO TEST //niszczy obiekt, usuwa ID z używanych i daje do nieużywanych

    ElementID get_id() const {return _ID;}

    operator=(Package&& prod); // @NOT IMPLEMENTED //??
private:
    ElementID _ID;
    static std::set<ElementID> _assigned_IDs;
    static std::set<ElementID> _freed_IDs;
};

#endif //NETSIM_PACKAGE_HPP
