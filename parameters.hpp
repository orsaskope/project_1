#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>
#include <cstdio>
using namespace std;

struct Params{
    FILE* input;    // Input file
    FILE* query;    // Output file
    FILE* o;        // Output file
    int k;          // Number of LSH functions for g
    int l;          // Number of hashtables
    double w;       // Cell size on the straight line
    int n;          // Number of nearest
    int r;          // Search radius
    string type;    // sift/mnist
    bool range;     // If false->No aerea search

    int kproj;      // Projection points
    int m;          // Max points to check(hypercube) OR number of subvectors(ivfpq)
    int probes;     // Max cube vertices

    int kclusters;  // Number of clusters
    int nprobe;     // Number of clusters to check
    int seed;       // For rand
    int nbits;      // 2^nbits subspaces
    int algorithm;  // lsh = 0, hypercube = 1, ivfflat = 2, ivfpq = 3

    Params();
};

void validArgument(char* argument, int argc, int i);
Params* ArgsParser(int argc, char* argv[]);
void initializeParams(Params* params);


#endif