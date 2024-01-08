#include "reports.hpp"
#include <algorithm>
void generate_structure_report(const Factory& f, std::ostream& os){
    std::vector<ElementID> ramp_id;
    os << "\n== LOADING RAMPS ==\n";
    for(auto r = f.ramp_cbegin(); r != f.ramp_cend(); ++r) {
        auto &ramp = *r;
        ramp_id.push_back(ramp.get_id());
    }
    std::sort(ramp_id.begin(),ramp_id.end());

    for(std::size_t counter=0;counter!=ramp_id.size();counter++){
        std::vector<ElementID> work_1;
        std::vector<ElementID> store_1;
        os << "\nLOADING RAMP #" << ramp_id[counter] << "\n";
        os << "  Delivery interval: " << f.find_ramp_by_id(ramp_id[counter])->get_delivery_interval()<< "\n";
        os << "  Receivers:\n";
        for (auto& rec: f.find_ramp_by_id(ramp_id[counter])->receiver_preferences_.get_preferences()) {
            if (rec.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
                store_1.push_back(rec.first->get_id());
            }
            if ((rec.first->get_receiver_type()) == ReceiverType::WORKER) {
                work_1.push_back(rec.first->get_id());
            }
        }
        std::sort(work_1.begin(),work_1.end());
        std::sort(store_1.begin(),store_1.end());
        if(!store_1.empty()){
            for(auto& rec: store_1){
                os<<"    storehouse #"<<rec<<"\n";
            }
        }
        if(!work_1.empty()) {
            for (auto &rec: work_1) {
                os << "    worker #" << rec << "\n";
            }
        }
    }

    os << "\n\n== WORKERS ==\n";
    std::vector<ElementID> work_vector;
    for(auto w = f.worker_cbegin(); w != f.worker_cend();w++) {
        auto &worker = *w;
        work_vector.push_back(worker.get_id());
    }
    std::sort(work_vector.begin(),work_vector.end());
    for(std::size_t counter=0;counter!=work_vector.size();counter++){
        std::vector<ElementID> work_2;
        std::vector<ElementID> store_2;
        os << "\nWORKER #" << work_vector[counter] << "\n";
        os << "  Processing time: " << f.find_worker_by_id(work_vector[counter])->get_processing_duration() << "\n";
        os << "  Queue type: ";
        for (auto& rec: f.find_ramp_by_id(work_vector[counter])->receiver_preferences_.get_preferences()) {
            if (rec.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
                store_2.push_back(rec.first->get_id());
            }
            if ((rec.first->get_receiver_type()) == ReceiverType::WORKER) {
                work_2.push_back(rec.first->get_id());
            }
        }
        std::sort(work_2.begin(),work_2.end());
        std::sort(store_2.begin(),store_2.end());
        if(f.find_worker_by_id(work_vector[counter])->get_queue()->get_queue_type() == PackageQueueType::FIFO){
            os << "FIFO\n";
        }
        else if(f.find_worker_by_id(work_vector[counter])->get_queue()->get_queue_type() == PackageQueueType::LIFO){
            os << "LIFO\n";
        }

        os << "  Receivers:\n";
        if(!store_2.empty()){
            for(auto& rec: store_2){
                os<<"    storehouse #"<<rec<<"\n";
            }
        }
        if(!work_2.empty()) {
            for (auto &rec: work_2) {
                os << "    worker #" << rec << "\n";
            }
        }
    }

    os << "\n\n== STOREHOUSES ==\n";
    std::vector<ElementID>store_vector;
    for(auto s = f.storehouse_cbegin(); s != f.storehouse_cend();s++) {
        auto &store = *s;
        store_vector.push_back(store.get_id());
    }
    std::sort(store_vector.begin(),store_vector.end());
    for(std::size_t counter=0;counter!=work_vector.size();counter++){
        os << "\nSTOREHOUSE #" << store_vector[counter] << "\n";
    }
    os << "\n";
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t){
    os << "=== [ Turn: " << t << " ] ===";
    
    os << "\n\n== WORKERS ==\n";
    for(auto w = f.worker_cbegin(); w != f.worker_cend(); w++){
        const auto &worker = *w;
        os << "\nWORKER #" << worker.get_id() << "\n";
            //PBUFFER
        os << "  PBuffer: ";
        if(worker.get_processing_buffer())
            os << '#' << worker.get_processing_buffer()->get_id() << ' ' << "(pt = " << 1+t-worker.get_package_processing_start_time() << ")";
        else
            os << "(empty)";
            //QUEUE
        os << "\n  Queue:";
        if(worker.get_queue()->size() > 0) {
            for (auto it = worker.cbegin(); it != worker.cend(); ++it) {
                os << " #" << it->get_id();
                if (std::next(it, 1) != worker.cend())
                    os << ',';
            }
        } else {
            os << " (empty)";
        }
            //SBUFFER
        os << "\n  SBuffer: ";
        if(worker.get_sending_buffer())
            os << '#' << worker.get_sending_buffer()->get_id() << '\n';
        else
            os << "(empty)\n";
    }

    os << "\n\n== STOREHOUSES ==\n";
    for(auto s = f.storehouse_cbegin(); s != f.storehouse_cend();s++){
        auto &store = *s;
        os << "\nSTOREHOUSE #" << store.get_id() << "\n";
        os << "  Stock:";
        if(store.cbegin() != store.cend()) {
            for (auto elem = store.cbegin(); elem != store.cend(); ++elem) {
                os << " #" << elem->get_id();
                if (std::next(elem, 1) != store.cend())
                    os << ',';
            }
        } else {
            os << " (empty)";
        }
    }
    os << "\n\n";
}

class IntervalReportNotifier{
public:
    IntervalReportNotifier(TimeOffset to) : _to(to) {};

    bool should_generate_report(Time t){
        if ((t-1) % _to == 0) { return true;}
        else { return false;}
    }
private:
    TimeOffset _to;
};

class SpecificTurnsReportNotifier{
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) : _turns(std::move(turns)) {}

    bool should_generate_report(Time t){
        auto it = _turns.find(t);
        if(it == _turns.cend()){ return false; }
        else { return true; }
    }
private:
    std::set<Time> _turns;
};
