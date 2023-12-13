#include "helpers.hpp"

#include <cstdlib>
#include <random>
#include <algorithm>

// Do generowania wysokiej jakości ciągów liczb pseudolosowych warto użyć
// zaawansowanych generatorów, np. algorytmu Mersenne Twister.
// zob. https://en.cppreference.com/w/cpp/numeric/random
std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    // Generuj liczby pseudolosowe z przedziału [0, 1); 10 bitów losowości.
    return std::generate_canonical<double, 10>(rng);
}

double single_vector_generator() {
    // Wartości z wektora brane są od początku (kolejka)
    static std::vector<double> pse_pse_liczby = {}; //WPISZ WEKTOR
    if(pse_pse_liczby.size() > 0){
        double x = pse_pse_liczby[0];
        pse_pse_liczby.erase(pse_pse_liczby.begin());
        if(x < 0)       //odwraca znak, jeśli zly
            x = -x;
        for(double iter = 0; x > 1; x/=10); //zabezpiecza, żeby liczba była z przedziału [0 1] jesli liczba jest za duza
        return x;
    }
    return 0;
}

double repeat_vector_generator() {
    static std::vector<double> pse_pse_liczby = {0.2, 0.1, 0.3, 0.7}; //WPISZ WEKTOR
    static std::vector<double> cur_queue;

    if(cur_queue.size() > 0){
        double x = cur_queue[0];
        cur_queue.erase(cur_queue.begin());
        return x;
    } else {
        std::copy(pse_pse_liczby.cbegin(), pse_pse_liczby.cend(), std::back_inserter(cur_queue));
        double x = cur_queue[0];
        cur_queue.erase(cur_queue.begin());
        return x;
    }
}

double single_value_generator() {
    double x = 0.2137;  //ZMIEŃ WARTOSC x
    if(x < 0 or x > 1)
        return 0;
    return x;
}

std::function<double()> probability_generator = default_probability_generator;

