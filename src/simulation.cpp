#include "simulation.hpp"

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf){
    for(Time t = 1; t == d+1; t++){
        if (!f.is_consistent()){ throw std::logic_error("Fabryka nie jest spójna");}
        f.do_deliveries(t);
        f.do_work(t);
        f.do_package_passing();
        rf(f,t);
    }
}