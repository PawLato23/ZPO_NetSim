#include "factory.hpp"

void Factory::do_package_passing() {
    for(auto& worker : _Worker_collection){
        worker.send_package();
    }
    for(auto& ramp : _Ramp_collection){
        ramp.send_package();
    }
}