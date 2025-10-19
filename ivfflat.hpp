#ifndef IVFFLAT_HPP
#define IVFFLAT_HPP

#include "parameters.hpp"

typedef vector<vector<float>> imagesVector;

struct IVFFLAT {
    int seed;
    int kclusters;
    int nprobe;
    int n;
    int r;

    IVFFLAT(int seed_, int kclusters_, int nprobe_, int n_, int r_);
};

void IvfflatSearch(imagesVector, IVFFLAT*);
void clustering(imagesVector, IVFFLAT*);
void IvfflatInit(imagesVector);


#endif