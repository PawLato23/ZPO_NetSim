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


enum Component {LOADING_RAMP, WORKER, STOREHOUSE, NODE};
struct ParsedLineData{
    Component TAG;
    std::map<std::string, std::string> params;
};

ParsedLineData parse_line(std::string line){
    ParsedLineData ParsLine;
    //Zmienienie linijki na odzdielne "wyrazy"
    std::istringstream token_stream(line);
    std::string *temp_str;
    std::vector<std::string> parameters;
    while(std::getline(token_stream, *temp_str, ' '))
        parameters.push_back(*temp_str);
    free( temp_str);
    //zapis typu
    if(parameters[0] == "LOADING_RAMP") {
        ParsLine.TAG = LOADING_RAMP;
    }else if(parameters[0] == "WORKER"){
        ParsLine.TAG = WORKER;
    }else if(parameters[0] == "STOREHOUSE"){
        ParsLine.TAG = STOREHOUSE;
    }else if(parameters[0] == "NODE")
        throw "error during recognition -> check if word-type is correct";
    //zapis pozostalych parametrow
    std::string key;
    std::string value;
    for(auto& iter : parameters){
        if(iter == parameters[0])
            continue;
        //oddzielenie klucz od wartości
        std::istringstream key_value(iter);
        std::getline(key_value, key, '=');
        std::getline(key_value, value, '=');
        //zapis do struktury ParsedLineData
        ParsLine.params[key] = value;
    }
    return (ParsLine);
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;

    std::string line;
    while (std::getline(is, line)) {
        // Pomijaj puste linie i linie zaczynające się od znaku komentarza
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Parsuj linię
        ParsedLineData parsed_data = parse_line(line);

        // W zależności od typu elementu wykonaj odpowiednie akcje
        switch (parsed_data.element_type) {
            case LOADING_RAMP: {
                // Utwórz obiekt Ramp i dodaj do fabryki
                Ramp ramp(parsed_data.parameters["ID"]);
                // Możesz dodać obsługę dodatkowych parametrów Ramp
                factory.add_ramp(ramp);
                break;
            }
            case WORKER: {
                // Utwórz obiekt Worker i dodaj do fabryki
                Worker worker(parsed_data.parameters["ID"],
                              std::stoi(parsed_data.parameters["ProcessingTime"]),
                              std::stoi(parsed_data.parameters["DeliveryInterval"]));
                // Możesz dodać obsługę dodatkowych parametrów Worker
                factory.add_worker(worker);
                break;
            }
            case STOREHOUSE: {
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
