#include <iostream>

using std::cout;
using std::endl;

template<class T>
bool EQUAL(T obj1, T obj2){
    if(obj1 == obj2)
        return true;
    else{
        std::cout<<"not equal on line: "<<__LINE__<<std::endl;
        return false;
    }
}
