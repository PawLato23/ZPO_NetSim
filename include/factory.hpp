#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "nodes.hpp"
#include <memory>

template <typename Node> class NodeCollection{
    using container_t = typename std::vector<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
public:
    void add(Node&& node) {node_vector_.push_back(std::move(node));}

    iterator find_by_id(ElementID id){
        auto it = std::find_if(node_vector_.begin(),node_vector_.end(),
                               [id](auto& elem){ return (elem.get_id() == id);});
        return it;
    }

    const_iterator find_by_id(ElementID id) const{
        auto it = std::find_if(node_vector_.cbegin(),node_vector_.cend(),
                               [id](const auto& elem){ return (elem.get_id() == id);});
        return it;
    }

    void remove_by_id(ElementID id){
        auto it = std::find_if(node_vector_.begin(),node_vector_.end(),
                               [id](auto& elem){ return (elem.get_id() == id);});
        if (it != node_vector_.end()){
            node_vector_.erase(it);
        }
    }

    const_iterator cbegin() const { return node_vector_.cbegin();}
    const_iterator cend() const { return node_vector_.cend();}
    const_iterator begin() const { return node_vector_.begin();}
    const_iterator end() const { return node_vector_.end();}
    iterator begin() { return node_vector_.begin();}
    iterator end() { return node_vector_.end();}
private:
    container_t node_vector_;
};

class Factory{
    
};

#endif //NETSIM_FACTORY_HPP
