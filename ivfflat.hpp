#ifndef IVFFLAT_HPP
#define IVFFLAT_HPP

#include "parameters.hpp"
#include <iostream>
#include <random>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <chrono>

typedef vector<vector<float>> imagesVector;
typedef vector<float> floatVec;

struct IVFFLAT {
    int seed;
    int kclusters;
    int nprobe;
    int n;
    int r;
    int image_size;

    imagesVector centroids; // The centroid's index in this vector, is the centroid's index in inverted_lists
    vector<imagesVector> inverted_lists;
    
    IVFFLAT(int seed_, int kclusters_, int nprobe_, int n_, int r_, int image_size_);
};

void updateCentroids(IVFFLAT*);
floatVec findMinDistanceToCentroids(imagesVector dataset, IVFFLAT* ivfflat);
void getNewCentroid(imagesVector dataset, IVFFLAT* ivfflat, floatVec D, std::default_random_engine&, int);
void printCLusters(IVFFLAT*);
void assignToNearestCentroid(imagesVector, IVFFLAT*);
void IvfflatSearch(imagesVector, IVFFLAT*, int range);
void clustering(imagesVector, IVFFLAT*);
void IvfflatInit(imagesVector);


#endif