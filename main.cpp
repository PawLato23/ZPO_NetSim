#include <iostream>

#include "src/package.cpp"
#include "test.cpp"
using std::cout;
using std::endl;

template<class T>
std::ostream& operator<<(std::ostream& out, const std::set<T> x){
    out<<"{ ";
    for(auto it=x.cbegin(); it!=x.cend(); it++)
        out<<*it<<' ';
    out<<'}';
    return out;
}

int main() {
    Package obj1 = Package(5);
    Package obj2 = Package(6);
    EQUAL(obj1.get_id(), ElementID (5));
    EQUAL(obj2.get_id(), ElementID (6));



    return 0;
}
