#include "factory.hpp"

void Factory::do_package_passing() {
    for(auto& worker : _Worker_collection){
        worker.send_package();
    }
    for(auto& ramp : _Ramp_collection){
        ramp.send_package();
    }
}
enum Color { NIEODWIEDZONY, ODWIEDZONY, ZWERYFIKOWANY };

bool is_storehouse_available(PackageSender* send, std::map<PackageSender*,Color>& map){
    auto it = map.find(send);
    if (it->second == Color::ZWERYFIKOWANY){
        return true;
    }
    it->second = Color::ODWIEDZONY;
    if(send->receiver_preferences_.get_preferences().empty()){
        return false;
    }
    bool different_send=false;
    for(auto pair : send->receiver_preferences_.get_preferences()){
        if(pair.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            different_send=true;
        }
        else if(pair.first->get_receiver_type() == ReceiverType::WORKER){
            IPackageReceiver* receiver_ptr = pair.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendrecv_ptr != send){
                different_send = true;
            }
            if(map.find(sendrecv_ptr)->second == Color::NIEODWIEDZONY){
                bool is = is_storehouse_available(sendrecv_ptr,map);
                if(!is){
                    return false;
                }
            }
        }
    }
    it->second=Color::ZWERYFIKOWANY;
    if(different_send){
        return true;
    }
    else{
        return false;
    }
}

bool Factory::is_consistent(){
    std::map<PackageSender*,Color> color_collection;
    for(auto &ramp : _Ramp_collection){
        color_collection[&ramp]=Color::NIEODWIEDZONY;
    }
    for(auto &worker : _Worker_collection){
        color_collection[&worker]=Color::NIEODWIEDZONY;
    }
    for(auto &ramp : _Ramp_collection){
        bool is = is_storehouse_available(&ramp, color_collection);
        if(!is){
            return false;
        }
    }
    for(auto &worker : _Worker_collection){
        bool is = is_storehouse_available(&worker, color_collection);
        if(!is){
            return false;
        }
    }
    return true;
}

struct ParsedLineData {
    ElementID element_type;
    std::map<std::string, std::string> parameters;
};

ParsedLineData parse_line(const std::string& line) {
    ParsedLineData parsed_data;

    // Implementacja parsowania linii na ID i pary (klucz, wartość)
    // ...

    // Przykładowa implementacja parsowania:
    std::istringstream iss(line);
    std::string id;
    iss >> id;

    // Reszta linii traktowana jako pary (klucz, wartość)
    std::map<std::string, std::string> parameters;
    std::string token;
    while (iss >> token) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            parameters[key] = value;
        }
    }

    // Ustawienie danych w strukturze ParsedLineData
    parsed_data.element_type = find_by_id(id);
    parsed_data.parameters = parameters;

    return parsed_data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;

    std::string line;
    while (std::getline(is, line)) {
        // Pomijaj puste linie i linie zaczynające się od znaku komentarza
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Parsuj linię
        ParsedLineData parsed_data = parse_line(line);

        // W zależności od typu elementu wykonaj odpowiednie akcje
        switch (parsed_data.element_type) {
            case NodeCollection::Ramp: {
                // Utwórz obiekt Ramp i dodaj do fabryki
                Ramp ramp(parsed_data.parameters["ID"]);
                // Możesz dodać obsługę dodatkowych parametrów Ramp
                factory.add_ramp(ramp);
                break;
            }
            case NodeCollection::Worker: {
                // Utwórz obiekt Worker i dodaj do fabryki
                Worker worker(parsed_data.parameters["ID"],
                              std::stoi(parsed_data.parameters["ProcessingTime"]),
                              std::stoi(parsed_data.parameters["DeliveryInterval"]));
                // Możesz dodać obsługę dodatkowych parametrów Worker
                factory.add_worker(worker);
                break;
            }
            case NodeCollection::Storehouse: {
                // Utwórz obiekt Storehouse i dodaj do fabryki
                Storehouse storehouse(parsed_data.parameters["ID"]);
                // Możesz dodać obsługę dodatkowych parametrów Storehouse
                factory.add_storehouse(storehouse);
                break;
            }
            default:
                // Nieznany typ, można obsłużyć błąd lub pominąć zależnie od potrzeb
                std::cerr << "Unknown element type" << std::endl;
        }
    }

    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os){

}
