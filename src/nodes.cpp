#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver *r) {
    std::size_t num_of_receivers = preferences_.size();
    if (num_of_receivers == 0){
        preferences_[r] = 1.0;
    }
    else{
        for (auto &receiver : preferences_){
            receiver.second = 1.0 / (double(num_of_receivers + 1));
        }
        preferences_[r] = 1.0 / (double(num_of_receivers + 1));
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r) {
    std::size_t num_of_receivers = preferences_.size();
    if (num_of_receivers > 1){
        for (auto &rec: preferences_) {
            if (rec.first != r) {
                rec.second = 1.0 / double((num_of_receivers - 1));
            }
        }
    }
    preferences_.erase(r);
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = pg_();
    if (p >= 0 && p <= 1) {
        double dist = 0.0;
        for (auto &rec: preferences_) {
            dist = dist + rec.second;
            if (dist < 0 || dist > 1) {
                return nullptr;
            }
            if (p <= dist) {
                return rec.first;
            }
        }
        return nullptr;
    }
    return nullptr;
}

void PackageSender::send_package() {
    IPackageReceiver *receiver;
    if (bufor_) {
        receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(*bufor_));
        bufor_.reset();
    }
}

void Ramp::deliver_goods(Time t) {
    if (!bufor_) {
        push_package(Package());
        bufor_.emplace(id_);
        t_ = t;
    } else {
        if (t - di_ == t_) {
            push_package(Package());
        }
    }
}

void Worker::do_work(Time t) {
    if (!bufor_ && !q_->empty()) {
        bufor_.emplace(q_->pop());
        t_ = t;
        if (t - t_ + 1 == pd_) {
            push_package(Package(bufor_.value().get_id()));
            bufor_.reset();
            if (!q_->empty()) {
                bufor_.emplace(q_->pop());
            }
        }

    } else {
        if (t - t_ + 1 == pd_) {
            push_package(Package(bufor_.value().get_id()));
            bufor_.reset();
            if (!q_->empty()) {
                bufor_.emplace(q_->pop());
            }
        }
    }
}

void Worker::receive_package(Package &&p) {
    q_->push(std::move(p));
}

void Storehouse::receive_package(Package &&p) {
    d_->push(std::move(p));
}