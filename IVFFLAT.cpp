#include "ivfflat.hpp"
#include <iostream>
#include <random>

using namespace std;

IVFFLAT::IVFFLAT(int seed_, int kclusters_, int nprobe_, int n_, int r_)
    :seed(seed_), kclusters(kclusters_), nprobe(nprobe_), n(n_), r(r_) {
    cout << "IVFFLAT constructor used" << endl;
}

void IvfflatSearch(imagesVector dataset, IVFFLAT* ivfflat) {
    clustering(dataset, ivfflat);
    return;
}

void clustering(imagesVector dataset, IVFFLAT* ivfflat) {
    int seed = ivfflat->seed;
    int kclusters = ivfflat->kclusters;
    int nprobe = ivfflat->nprobe;
    int n = ivfflat->n;
    int r = ivfflat->r;

    default_random_engine generator(seed);
    uniform_int_distribution<int> dist(0, dataset.size() - 1);
    return;
}

void IvfflatInit(imagesVector dataset) {
    return;
}