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
        switch (parsed_data.TAG) {
            case LOADING_RAMP: {
                // Utwórz obiekt Ramp i dodaj do fabryki
                Ramp ramp{(ElementID)stoi(parsed_data.params["id"]), (ElementID)stoi(parsed_data.params["delivery-interval"])};
                // Możesz dodać obsługę dodatkowych parametrów Ramp
                factory.add_ramp(std::move(ramp));
                break;
            }
            case WORKER: {
                // definicja bufora na półprodukty i jej typu
                PackageQueueType qt;
                if(parsed_data.params["queue-type"] == "FIFO")
                    qt = PackageQueueType::FIFO;
                else if(parsed_data.params["queue-type"] == "LIFO")
                    qt = PackageQueueType::LIFO;

                std::unique_ptr<IPackageQueue> q;

                Worker worker((ElementID)stoi(parsed_data.params["id"]),
                              (TimeOffset)stoi(parsed_data.params["processing-time"]),
                              std::move(q));
                // Możesz dodać obsługę dodatkowych parametrów Worker
                factory.add_worker(std::move(worker));
                break;
            }
            case STOREHOUSE: {
                // Utwórz obiekt Storehouse i dodaj do fabryki
                Storehouse storehouse(std::stoi(parsed_data.params["id"]));
                // Możesz dodać obsługę dodatkowych parametrów Storehouse
                factory.add_storehouse(std::move(storehouse));
                break;
            }
            case LINK: {
                std::istringstream link_fr(parsed_data.params["src"]);
                std::istringstream link_to(parsed_data.params["dest"]);
                std::string src_type, des_type, src_id, des_id;
                //dzielenie na typ i id
                std::getline(link_fr, src_type, '-');
                std::getline(link_fr, src_id, '-');
                std::getline(link_to, des_type, '-');
                std::getline(link_to, des_id, '-');
                //dodwanie połączeń
                if(src_type == "LOADING_RAMP"){
                    auto sender = factory.find_ramp_by_id((ElementID)stoi(src_id));
                    if(des_type == "WORKER") {
                        IPackageReceiver *reciver = factory.find_worker_by_id((ElementID) stoi(des_id)); //nw jak
                        sender->receiver_preferences_.add_receiver(reciver);
                    }
                    else if(des_type == "STOREHOUSE"){
                        IPackageReceiver *reciver = factory.find_storehouse_by_id((ElementID) stoi(des_id)); //nw jak
                        sender->receiver_preferences_.add_receiver(reciver);
                    }
                } else if(src_type == "WORKER") {
                    auto sender = factory.find_worker_by_id((ElementID) stoi(src_id));
                    if (des_type == "WORKER") {
                        IPackageReceiver *reciver = factory.find_worker_by_id((ElementID) stoi(des_id)); //nw jak
                        sender->receiver_preferences_.add_receiver(reciver);
                    } else if (des_type == "STOREHOUSE") {
                        IPackageReceiver *reciver = factory.find_worker_by_id((ElementID) stoi(des_id)); //nw jak
                        sender->receiver_preferences_.add_receiver(reciver);
                    }
                    break;
                }
            }
                default:{
                        // Nieznany typ, można obsłużyć błąd lub pominąć zależnie od potrzeb
                        std::cerr << "Unknown element type" << std::endl;
                }
        }

    }
    if(factory.is_consistent())
        return factory;
    else
        throw "ajaj";
}


void save_factory_structure(Factory& factory, std::ostream& os){
    for(auto i = factory.ramp_cbegin(); i != factory.ramp_cend(); ++i) {
        const auto &ramp = *i;
        ramp.get_delivery_interval();
        ramp.receiver_preferences_.get_preferences();
    }
    os << "== WORKERS == \n";
    for(auto i = factory.worker_cbegin(); i != factory.worker_cend(); ++i){
        const auto& worker = *i;
        worker.get_package_processing_start_time();
        worker.receiver_preferences_.get_preferences();

    }
    os << "== STOREHOUSES == \n";
    for(auto i = factory.storehouse_cbegin(); i != factory.storehouse_cend(); ++i) {
        const auto &storehouse = *i;
        os << "STOREHOUSE #" << storehouse.get_id() << "\n";
        /*os << "  Stock: ";
        for(auto j = storehouse.cbegin(); j != storehouse.cend(); ++j){
            const auto &elem = *j;
            os <<" #" << elem.get_id();
            if (j != storehouse.cend()--){
                os << ",";
            }
        }*/
    }
}
