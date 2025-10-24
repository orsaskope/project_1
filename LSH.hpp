#ifndef LSH_HPP
#define LSH_HPP

#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <sstream>
#include <cmath>
#include <iostream>



// === Locality Sensitive Hashing (LSH) class ===
class LSH {
public:

    struct GResult {
        int index;                 // index of the vector in dataset
        std::string g_key;         // string key (for reference)
        unsigned long long ID;     // locality-sensitive ID
    };


    int L;   // number of hash tables
    int k;   // number of hash functions per table
    int dim; // dimension of data
    double w; // bucket width
    unsigned seed;

    // Random parameters
    std::vector<std::vector<std::vector<double>>> a; // [L][k][dim]
    std::vector<std::vector<double>> b;              // [L][k]

    // In LSH.h
    std::vector<std::vector<long long>> r;  // L x k random integer coefficients


    // Hash tables
    std::vector<std::unordered_map<std::string, std::vector<GResult>>> hashTables;


    // Constructor
    LSH(int L_, int k_, int dim_, double w_, unsigned seed_ = 1);

    // Hashing functions
    std::vector<long long> compute_h(const std::vector<double>& point, int tableIdx );

    GResult compute_g(const std::vector<double>& point, int tableIdx, int n, int index);
    // std::string compute_g(const std::vector<double>& point, int tableIdx , int n);

    void insert(int index, const std::vector<double>& point , int n);
    void print_tables();


private:
    std::mt19937_64 rng;
};

#endif
