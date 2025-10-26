#include "ivfflat.hpp"


using namespace std;

IVFFLAT::IVFFLAT(int seed_, int kclusters_, int nprobe_, int n_, int r_, int image_size_)
    :seed(seed_), kclusters(kclusters_), nprobe(nprobe_), n(n_), r(r_), image_size(image_size_) {
    centroids.reserve(kclusters);
    cout << "IVFFLAT constructor used" << endl;
}

void IvfflatSearch(imagesVector dataset, IVFFLAT* ivfflat, int range) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    // cout << "printing dataset" << endl;
    // for (int j = 0; j < dataset.size(); j++) {
    //     for (int i = 0; i < dataset[j].size(); i++)
    //         cout << dataset[j][i] << " ";
    //     cout << endl;
    // }

    clustering(dataset, ivfflat);
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    cout << "\nTraining completed in " << elapsed.count() << " seconds." << endl;
    if (!range) return;

    cout << "searching.." << endl;
    return;
}

// tbd: ?xreiazetai sqrt? afoy me noiazei h sygkrish apostasewn k dn me noiazei h akrivhs apostash
float euclideanDist(floatVec& a, floatVec& b, int image_size) {
    float dist = 0.0;
    for (int i = 0; i < image_size; i++) 
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(dist);
}

void clustering(imagesVector dataset, IVFFLAT* ivfflat) {
    int seed = ivfflat->seed;
    int kclusters = ivfflat->kclusters;
    int nprobe = ivfflat->nprobe;
    int n = ivfflat->n;
    int r = ivfflat->r;

    // initializing centroids by chosing "kclusters" random images. 
    // Generate random num for index.
    default_random_engine generator(seed);
    uniform_int_distribution<int> dist(0, dataset.size() - 1);

    //unordered_set<int> indexes; // will hold the index of the selected centroids
    ivfflat->inverted_lists.resize(ivfflat->kclusters);

    int idx = dist(generator);
    //indexes.insert(idx);
    ivfflat->centroids.push_back(dataset[idx]);
    ivfflat->inverted_lists[0].push_back(dataset[idx]);

    // for (int j = 0; j < ivfflat->centroids.size(); j++) {
    //     for (int i = 0; i < ivfflat->centroids[j].size(); i++)
    //         cout << ivfflat->centroids[j][i] << " ";
    //     cout << endl;
    // }
    //assignToNearestCentroid(dataset, ivfflat);

    for (int i = 1; i < kclusters; i++) {
        floatVec D = findMinDistanceToCentroids(dataset, ivfflat);
        getNewCentroid(dataset, ivfflat, D, generator, i);
    }
    assignToNearestCentroid(dataset, ivfflat);

    for (int i = 0; i < 10; i++) {
        imagesVector old_centroids = ivfflat->centroids;
        float change = 0.0;

        updateCentroids(ivfflat);
        assignToNearestCentroid(dataset, ivfflat);

        for (int i = 0; i < ivfflat->kclusters; i++)
            change += euclideanDist(old_centroids[i], ivfflat->centroids[i], ivfflat->image_size);
        if (change < 1e-3f) break;
    }
    

    printCLusters(ivfflat);
    return;
}

// Iterate through all the vectors and compute their distance to their closest centroid. This will help compute the probability
// of every vector becoming a centroid. We keep the distances squared for the probability function P(Xi) = D(i)^2/ΣD(j)
floatVec findMinDistanceToCentroids(imagesVector dataset, IVFFLAT* ivfflat) {
    floatVec D; // Will hold the distance of every vector to its nearest centroid (squared).
    for (size_t i = 0; i < dataset.size(); i++) {
        // If current vector is a centroid, update D with 0.
        if (std::find(ivfflat->centroids.begin(), ivfflat->centroids.end(), dataset[i]) != ivfflat->centroids.end()) {
            D.push_back(0);
            continue;
        }

        // Vector is not a centroid
        float dist = std::numeric_limits<float>::max();
        for (size_t j = 0; j < ivfflat->centroids.size(); j++) {
            float curr_dist = euclideanDist(dataset[i], ivfflat->centroids[j], ivfflat->image_size);
            if (curr_dist < dist) dist = curr_dist;
        }
        D.push_back(dist * dist);
    }
    return D;
}

void getNewCentroid(imagesVector dataset, IVFFLAT* ivfflat, floatVec D, std::default_random_engine& generator, int idx) {
    floatVec p_sums;    // Wil hold the distance to closest vector of ALL the previous vectors and the current one.
    float dist_sum = 0.0f;
    for (size_t i = 0; i < D.size(); i++) {
        dist_sum += D[i];
        p_sums.push_back(dist_sum);
    }
    
    float total_sum = p_sums.back();    // Total sum of all vectors' distances, for random index initialization.
    for (size_t i = 0; i < D.size(); i++) {
        p_sums[i] /= total_sum;
    }

    std::uniform_real_distribution<float> dis(0.0, 1.0);
    float x = dis(generator);

    auto iter = lower_bound(p_sums.begin(), p_sums.end(), x);
    int r = iter - p_sums.begin();

    ivfflat->centroids.push_back(dataset[r]);
    ivfflat->inverted_lists[idx].push_back(dataset[r]);
    cout << "Chosen centroid index: " << r << endl;
}

// Assigns every vector to its closest centroid.
void assignToNearestCentroid(imagesVector dataset, IVFFLAT* ivfflat) {
    ivfflat->inverted_lists.clear();
    ivfflat->inverted_lists.resize(ivfflat->kclusters);
    
    // iterate through every vector and find the closest centroid. Assign vector to the corresponding cluster.
    for (size_t i = 0; i < dataset.size(); i++) {
        // Initialize dist and nearest centroid so that we can find minimum distance and assign current vector to closest centroid's cluster.
        float dist = std::numeric_limits<float>::max(); 
        size_t nearest = 0; 
        floatVec a = dataset[i];

        if (std::find(ivfflat->centroids.begin(), ivfflat->centroids.end(), a) != ivfflat->centroids.end()) continue;

        for (size_t j = 0; j < ivfflat->centroids.size(); j++) {
            floatVec b = ivfflat->centroids[j];  // got current centroid
            float temp_dist = euclideanDist(a, b, ivfflat->image_size);
            if (temp_dist < dist) {
                dist = temp_dist;
                nearest = j;
            }
        }
        ivfflat->inverted_lists[nearest].push_back(a);
    }    
}

void updateCentroids(IVFFLAT* ivfflat) {
    for (int i = 0; i < ivfflat->kclusters; i++) {
        imagesVector curr_cl = ivfflat->inverted_lists[i]; // Got current cluster
        if (curr_cl.empty())    continue;
        
        // This vector will hold the average of the current clusters' vectors coordinates
        floatVec new_cntr;
        new_cntr.resize(ivfflat->image_size, 0.0f);

        for (size_t j = 0; j < curr_cl.size(); j++) {
            for (size_t k = 0; k < curr_cl[j].size(); k++) {
                new_cntr[k] += curr_cl[j][k];
            }
        }
        for (int k = 0; k < ivfflat->image_size; k++) {
            new_cntr[k] /= curr_cl.size();
        }
        ivfflat->centroids[i] = new_cntr;
    }
    
}



void printCLusters(IVFFLAT* ivfflat) {
    cout << ivfflat->kclusters << " clusters , " << ivfflat->centroids.size() << " centroids." << endl;
    cout << "centroids' size " << ivfflat->centroids.size() << endl;
    //for (size_t j = 0; j < ivfflat->centroids.size(); j++) {
      //  for (size_t i = 0; i < ivfflat->centroids[j].size(); i++)
        //    cout << ivfflat->centroids[j][i] << " ";
        //cout << endl;
    //}
    cout << "inverted list size: 1." << ivfflat->inverted_lists[0].size() << "\n2." << ivfflat->inverted_lists[1].size() << "\n3." << ivfflat->inverted_lists[2].size() << "\n4." << ivfflat->inverted_lists[3].size() << endl;
    cout << "seed: " << ivfflat->seed << endl;
}

void IvfflatInit(imagesVector dataset) {
    return;
}