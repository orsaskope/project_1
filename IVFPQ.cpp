#include "ivfpq.hpp"


using namespace std;

IVFPQ::IVFPQ(int seed_, int kclusters_, int nprobe_, int n_, int r_, int image_size_, int m_, int nbits_)
    :seed(seed_), kclusters(kclusters_), nprobe(nprobe_), n(n_), r(r_), image_size(image_size_), m(m_), nbits(nbits_) {
    centroids.reserve(kclusters);
    cout << "IVFpq constructor used" << endl;
}

static float euclideanDist(floatVec& a, floatVec& b, int image_size) {
    float dist = 0.0;
    for (int i = 0; i < image_size; i++) 
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(dist);
}

static bool comparePairs(pair<int, float> a, pair<int, float> b) {
    return a.second < b.second;
}


void IvfpqSearch(imagesVector& dataset, IVFPQ* ivfpq, imagesVector queryfile, string output) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    FILE* fout = fopen(output.c_str(), "w");
    if (!fout) {
        perror("Failed to open output file");
        exit(errno);
    }
    fprintf(fout, "IVFPQ\n");

    clustering(dataset, ivfpq);
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    cout << "\nTraining completed in " << elapsed.count() << " seconds." << endl;
   
    double total_af = 0.0;          // Sum of all Approximation Factors (distanceApproximate / distanceTrue)
    double total_recall = 0.0;      // Sum of all the true nearest neighbours that ivfpq found/N
    double total_ivfpq_time = 0.0;   // Total IVFPQ time
    double total_brf_time = 0.0;    // Total brute force time
    int Q = queryfile.size();       // Queries searched
    int N = ivfpq->n;               // Nearest neighbours
    cout << "searching.." << endl;
    
    for (size_t i = 0; i < queryfile.size(); i++) {
        fprintf(fout, "Query: %zu\n", i);
        
        // Search for queries' approximate nearest neighbours, count time.
        auto t1 = high_resolution_clock::now();
        vector<int> top_clusters = QueryCentroidSearch(ivfpq, queryfile[i]);
        pair<vector<pair<int, float>>, vector<int>> results = QueryVectorSearch(ivfpq, queryfile[i], i, top_clusters, fout, dataset);
       
        vector<pair<int, float>> nn_res = results.first;
        vector<int> range_res = results.second;
       
        auto t2 = high_resolution_clock::now();
        double curr_ann_time = duration<double>(t2 - t1).count();
        total_ivfpq_time += curr_ann_time;   // Add the current time to the total

        // Search for queries actual nearest neighbours, cout time.
        auto t3 = high_resolution_clock::now();
        vector<pair<int, float>> brute_res = bruteForce(ivfpq, queryfile[i], i, fout, dataset);
        auto t4 = high_resolution_clock::now();
        double curr_true_time = duration<double>(t4 - t3).count();
        total_brf_time += curr_true_time;

        double curr_af = nn_res[0].second / brute_res[0].second;
        total_af += curr_af;

        // --- Recall@N ---
        double recall = 0.0;
        int correct = 0;
        for (int ivfpq_n = 0; ivfpq_n < N; ivfpq_n++) {
            int ivfpq_idx = nn_res[ivfpq_n].first;
            for (int brute_n = 0; brute_n < N; brute_n++) {
                if (brute_res[brute_n].first == ivfpq_idx) {
                    correct++;
                    break;
                }
            }
        }
        recall = (double)correct / N;
        total_recall += recall;

        for (int j = 0; j < ivfpq->n; ++j) {
            fprintf(fout, "Nearest neighbor-%d: %d\n", j + 1, nn_res[j].first);
            fprintf(fout, "distanceApproximate: %.6f\n", nn_res[j].second);
            fprintf(fout, "distanceTrue: %.6f\n", brute_res[j].second);
        }
        if (ivfpq->r) {
                fprintf(fout, "R-near neighbors:\n");
                for (size_t i = 0; i < range_res.size(); i++)
                    fprintf(fout, "%d\n",range_res[i]);
        }
    }
    fprintf(fout, "Average AF: %.6f\n", total_af / Q);
    fprintf(fout, "Recall@N: %.6f\n", total_recall / Q);
    fprintf(fout, "QPS: %.6f\n", Q / total_ivfpq_time);
    fprintf(fout, "tApproximateAverage: %.6f\n", total_ivfpq_time / Q);
    fprintf(fout, "tTrueAverage: %.6f\n", total_brf_time / Q);

    fclose(fout);
    return;
}

vector<int> QueryCentroidSearch(IVFPQ* ivfpq, floatVec q) {
    vector<pair<int,float>>centroids_dist;
    
    for (int i = 0; i < ivfpq->kclusters; i++) {
        float d = euclideanDist(q, ivfpq->centroids[i], ivfpq->image_size);
        centroids_dist.push_back({i, d});
    }
    sort(centroids_dist.begin(), centroids_dist.end(), comparePairs);   // Sorting to find the closest

    vector<int> top_clusters;
    for (int j = 0; j < ivfpq->nprobe; j++)
        top_clusters.push_back(centroids_dist[j].first);    // Get the nprobe closest
    return top_clusters;
}

pair<vector<pair<int, float>>, vector<int>> QueryVectorSearch(IVFPQ* ivfpq, floatVec q, int q_idx, vector<int> top_clusters, FILE* fout, imagesVector& dataset) {
    vector<pair<int, float>> results;   // Index, distance
    vector<int> range_res;              // Index in range R
    
    // For every cluster from top clusters:
    for (size_t i = 0; i < top_clusters.size(); i++) {
        int curr_cl = top_clusters[i];
        
        // Compure residuals
        floatVec query_r(ivfpq->image_size);
        for (int d = 0; d < ivfpq->image_size; d++)
            query_r[d] = q[d] - ivfpq->centroids[curr_cl][d];
        
        vector<vector<float>> dist_table = DistanceTables(ivfpq, query_r);

        // Compute approximate distance using the pq codes
        for (size_t v = 0; v < ivfpq->codes[curr_cl].size(); v++) {
            vector<int> code = ivfpq->codes[curr_cl][v];    // Get pq code for every vector
            float total_dist = 0.0f;

            // For every subspace, we take the corresponding codeword index and we add the distance table value 
            for (int sub = 0; sub < ivfpq->m; sub++) {
                int codeword_idx = code[sub];
                total_dist += dist_table[sub][codeword_idx];
            }

            float approx_dist = sqrt(total_dist);
            int index = ivfpq->idVec[curr_cl][v].first; // Get the real index
            results.push_back({index, approx_dist});

            if (ivfpq->r > 0 && approx_dist < ivfpq->r)
                range_res.push_back(index);
        }
    }
    sort(results.begin(), results.end(), comparePairs);
    if (results.size() > ivfpq->n)
        results.resize(ivfpq->n);
    return {results, range_res};
}

vector<vector<float>> DistanceTables(IVFPQ* ivfpq, floatVec query_r) {
    int subsp_num = ivfpq->m;
    int subsp_sz = ivfpq->image_size / subsp_num;
    int s = (int)pow(2, ivfpq->nbits);

    vector<vector<float>> dist_table(subsp_num, vector<float>(s, 0.0f));    // Will hold the distance for all nprobe centroids
    vector<floatVec> query_subs(subsp_num, floatVec(subsp_sz));             // Query residuals / m
    // Split the query residuals
    for (int sub = 0; sub < subsp_num; sub++) {
        int offset = sub * subsp_sz;
        for (int d = 0; d < subsp_sz; d++)
            query_subs[sub][d] = query_r[offset + d];
    }
    // Find distance form codeword
    for (int sub = 0; sub < subsp_num; sub++) {
        for (int codeword = 0; codeword < s; codeword++) {
            float d = 0.0f;
            for (int i = 0; i < subsp_sz; i++) {
                float diff = query_subs[sub][i] - ivfpq->codebooks[sub][codeword][i];
                d += diff * diff;  // squared distance
            }
dist_table[sub][codeword] = d;
            // dist_table[sub][codeword] = euclideanDist(query_subs[sub], ivfpq->codebooks[sub][codeword], subsp_sz);
        }
    }

    return dist_table;
}


void clustering(imagesVector& dataset, IVFPQ* ivfpq) {
    int seed = ivfpq->seed;
    int kclusters = ivfpq->kclusters;

    // initializing centroids by chosing "kclusters" random images. 
    // Generate random num for index.
    default_random_engine generator(seed);
    uniform_int_distribution<int> dist(0, dataset.size() - 1);

    imagesVector sample;

    int sample_sz = sqrt(dataset.size());
    sample.reserve(sample_sz);

    vector<int> sample_idx;
    while(sample.size() < sample_sz) {
        int idx = dist(generator);
        if (find(sample_idx.begin(), sample_idx.end(), idx) == sample_idx.end()) {
            sample.push_back(dataset[idx]);
            sample_idx.push_back(idx);
        }
    }

    ivfpq->inverted_lists.resize(ivfpq->kclusters);
    ivfpq->idVec.resize(ivfpq->kclusters);

    int idx = dist(generator) % sample.size();
    ivfpq->centroids.push_back(sample[idx]);
    ivfpq->inverted_lists[0].push_back(sample[idx]);

    /*For the initialization of the first kclusters centroids:
    The vector that is furthest from its closest centroid has better chances to
    become a centroid. When we choose the centroids, assign the vectors to the
    corresponding clusters */
    for (int i = 1; i < kclusters; i++) {
        cout << "Training..." << endl;
        floatVec D = findMinDistanceToCentroids(sample, ivfpq);
        getNewCentroid(sample, ivfpq, D, generator, i);
    }

    // After the initialization, assign vectors to their nearest centroid and
    // recalculate centroids. Repeat until there are not many changes.
     // After the initialization, assign vectors to their nearest centroid and
    // recalculate centroids. Repeat until there are not many changes.
    assignToNearestCentroid(dataset, ivfpq);
    for (int i = 0; i < 15; i++) {
        imagesVector old_centroids = ivfpq->centroids;
        float change = 0.0f;

        updateCentroids(ivfpq);
        assignToNearestCentroid(dataset, ivfpq);
        
        for (int i = 0; i < ivfpq->kclusters; i++)
        change += euclideanDist(old_centroids[i], ivfpq->centroids[i], ivfpq->image_size);
        float avg_movement = change / ivfpq->kclusters;
        cout << "Iteration " << i << " | avg_movement=" << avg_movement << endl;
        if (avg_movement < 20) break;
    }

    trainPQ(ivfpq, dataset);

    return;
}

// For every vector in the dataset. since its already assigned in its cluster:
// Iterate through the clusters and compute the distance from the clusters' centroid to current vector
// Store this difference in the residuals vector
void trainPQ(IVFPQ* ivfpq, imagesVector dataset) {
    cout << "got in train pq" << endl;
    vector<vector<float>> residuals;        // Will hold all the residuals (difference from corresponding centroid)
    ivfpq->codebooks.resize(ivfpq->m);      // Will hold the residuals of every subspace
    ivfpq->codes.resize(ivfpq->kclusters);  // Will hold

    // Itterate through the clusters and compute all residuals
    for (size_t j = 0; j < ivfpq->kclusters; j++) {
        for (size_t v = 0; v < ivfpq->inverted_lists[j].size(); v++) {
            floatVec vec = ivfpq->inverted_lists[j][v];   // Current vector
            floatVec r(vec.size());                       // Vector that will hold current vectors' residual
            
            // Compute difference from centroid's coordinates and store
            for (size_t k = 0; k < vec.size(); k++)
                r[k] = vec[k] - ivfpq->centroids[j][k];
            residuals.push_back(r);
        }
    }

    int subsp_sz = ivfpq->image_size / ivfpq->m;// To split the residuals vector in m subspaces.
    vector<vector<floatVec>> subspaces;         // Wil store the residuals. subspaces[i] = The subvectors of subspace i
    subspaces.resize(ivfpq->m);


    // Split the residuals and store them in the right subspace of subspaces vector.
    for (size_t idx = 0; idx < residuals.size(); idx++) {
        // For every residual:
        floatVec r = residuals[idx];
        for (int curr_sub = 0; curr_sub < ivfpq->m; curr_sub++) {
            floatVec sub;       // New subvector for every subspace
            sub.reserve(subsp_sz);    
            
            for (int i = curr_sub * subsp_sz; i < (curr_sub + 1) * subsp_sz; i++)
                sub.push_back(r[i]);
            subspaces[curr_sub].push_back(sub);
        }
    }
    cout << "computed residuals for all dataset" << endl;

    // Codebook
    int s = (int)pow(2, ivfpq->nbits);  // subspace centroids: 2^nbits
    for (int i = 0; i < ivfpq->m; i++) {
        cout << "splitting in subspaces" << endl;
        ivfpq->codebooks[i] = clusteringSubspaces(ivfpq, subspaces[i], s,subsp_sz);
    }

    ivfpq->codes.resize(ivfpq->kclusters);
    cout << "training codebooks" << endl;
    for (int i = 0; i < ivfpq->kclusters; i++) {
        // Curent cluster
        for(size_t v = 0; v < ivfpq->inverted_lists[i].size(); v++) {
            // Get the entire vector to compute residual r(x) = x - centroid
            const floatVec& vec = ivfpq->inverted_lists[i][v];
            floatVec residual(vec.size());

            for (size_t j = 0; j < vec.size(); j++)
                residual[j] = vec[j] - ivfpq->centroids[i][j]; // Difference from current centroid

            vector<floatVec> subresiduals(ivfpq->m, floatVec(subsp_sz, 0.0f));

            // Split residual vector into the right subspaces
            for (int subsp = 0; subsp < ivfpq->m; subsp++) {
                int offset = subsp * subsp_sz;
                for (int d = 0; d < subsp_sz; d++)
                    subresiduals[subsp][d] = residual[offset + d];
            }

            vector<int>current_code(ivfpq->m, 0);
            for (int subsp = 0; subsp < ivfpq->m; subsp++) {
                // For every subspace keep the closest codeword of codebook
                floatVec subvec = subresiduals[subsp];
                float min_dist = std::numeric_limits<float>::max();
                int closest = 0;

                for (size_t j = 0; j < ivfpq->codebooks[subsp].size(); j++) {
                    float dist = euclideanDist(subvec, ivfpq->codebooks[subsp][j], subsp_sz);
                    if (dist < min_dist) {
                        min_dist = dist;
                        closest = j;
                    }
                }
                current_code[subsp] = closest;
            }
            ivfpq->codes[i].push_back(current_code);
        }
    }
}

// Create the codebook. clustering *one* subspace 
vector<floatVec> clusteringSubspaces(IVFPQ* ivfpq, vector<floatVec> subspace, int s, int subsp_sz) {
    if (subspace.empty()) return {};

    default_random_engine gen(ivfpq->seed); // For centroids' initialization
    uniform_int_distribution<int> dist(0, subspace.size() - 1);

    vector<floatVec> centroids;
    centroids.reserve(s);   // 2^nbits

    // Initialization similar to ivf clustering
    vector<int> used;   // So that we don't chose a centroid more than once
    while (centroids.size() < s) {
        int idx = dist(gen);
        if (find(used.begin(), used.end(), idx) == used.end()) {
            centroids.push_back(subspace[idx]);
            used.push_back(idx);
        }
    }

    for (int i = 0; i < 10; i++) {
        vector<floatVec> buff_centroids(s, floatVec(subsp_sz, 0.0f));
        vector<int> cluster_count(s, 0);

        // For every subvector, we find the closest centroid
        for (size_t current = 0; current < subspace.size(); current++) {
            floatVec subvec = subspace[current]; // Got current subvector
            
            int closest = 0;                            // Closest centroid's index
            float closest_dist = std::numeric_limits<float>::max();
            for (int j = 0; j < s; j++) {
                float dist = euclideanDist(subvec, centroids[j], subsp_sz);
                if (dist < closest_dist) {
                    closest_dist = dist;
                    closest = j;
                }
            }
            // To find the new centroid, add all the subvectors that belong to this centroid
            for (int i = 0; i < subsp_sz; i++)
                buff_centroids[closest][i] += subvec[i];
            cluster_count[closest]++;
        }
        for (int i = 0; i < s; i++) {
            if (cluster_count[i] > 0) {
                // Compute new centroid, dividing all the sum with the count of subvectors that belong in the cluster
                for (int j = 0; j < subsp_sz; j++)
                    buff_centroids[i][j] /= cluster_count[i];
            } else {
                int idx = dist(gen);
                centroids[i] = subspace[idx];
            }
        }
        centroids = buff_centroids;
    }
    return centroids;
}


void getNewCentroid(imagesVector& dataset, IVFPQ* ivfpq, floatVec D, std::default_random_engine& generator, int idx) {
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

    ivfpq->centroids.push_back(dataset[r]);
    // ivfpq->inverted_lists[idx].push_back(dataset[r]);
    cout << "Chosen centroid index: " << r << endl;
}

// Assigns every vector to its closest centroid.
void assignToNearestCentroid(imagesVector& dataset, IVFPQ* ivfpq) {
    ivfpq->inverted_lists.clear();
    ivfpq->inverted_lists.resize(ivfpq->kclusters);
    
    ivfpq->idVec.clear();
    ivfpq->idVec.resize(ivfpq->kclusters);
    
    // iterate through every vector and find the closest centroid. Assign vector to the corresponding cluster.
    for (size_t i = 0; i < dataset.size(); i++) {
        // Initialize dist and nearest centroid so that we can find minimum distance and assign current vector to closest centroid's cluster.
        float dist = std::numeric_limits<float>::max(); 
        size_t nearest = 0; 
        floatVec a = dataset[i];

        
        if (std::find(ivfpq->centroids.begin(), ivfpq->centroids.end(), a) != ivfpq->centroids.end()) continue;
        
        for (size_t j = 0; j < ivfpq->centroids.size(); j++) {
            floatVec b = ivfpq->centroids[j];  // got current centroid
            float temp_dist = euclideanDist(a, b, ivfpq->image_size);
            if (temp_dist < dist) {
                dist = temp_dist;
                nearest = j;
            }
        }
        ivfpq->inverted_lists[nearest].push_back(a);
        ivfpq->idVec[nearest].push_back({i, a});
    }    
}

void updateCentroids(IVFPQ* ivfpq) {
    for (int i = 0; i < ivfpq->kclusters; i++) {
        imagesVector curr_cl = ivfpq->inverted_lists[i]; // Got current cluster
        if (curr_cl.empty())    continue;
        
        // This vector will hold the average of the current clusters' vectors coordinates
        floatVec new_cntr;
        new_cntr.resize(ivfpq->image_size, 0.0f);

        for (size_t j = 0; j < curr_cl.size(); j++) {
            for (size_t k = 0; k < curr_cl[j].size(); k++) {
                new_cntr[k] += curr_cl[j][k];
            }
        }
        for (int k = 0; k < ivfpq->image_size; k++) {
            new_cntr[k] /= curr_cl.size();
        }
        ivfpq->centroids[i] = new_cntr;
    }
    
}


// Iterate through all the vectors and compute their distance to their closest centroid. This will help compute the probability
// of every vector becoming a centroid. We keep the distances squared for the probability function P(Xi) = D(i)^2/ΣD(j)
floatVec findMinDistanceToCentroids(imagesVector& dataset, IVFPQ* ivfpq) {
    floatVec D; // Will hold the distance of every vector to its nearest centroid (squared).
    for (size_t i = 0; i < dataset.size(); i++) {
        // If current vector is a centroid, update D with 0 (so we don't lose indexes).
        if (std::find(ivfpq->centroids.begin(), ivfpq->centroids.end(), dataset[i]) != ivfpq->centroids.end()) {
            D.push_back(0);
            continue;
        }

        // Vector is not a centroid
        float dist = std::numeric_limits<float>::max();
        for (size_t j = 0; j < ivfpq->centroids.size(); j++) {
            float curr_dist = euclideanDist(dataset[i], ivfpq->centroids[j], ivfpq->image_size);
            if (curr_dist < dist) dist = curr_dist;
        }
        D.push_back(dist * dist);
    }
    return D;
}


vector<pair<int,float>> bruteForce(IVFPQ* ivfpq, floatVec q, int q_idx, FILE* fout, imagesVector& dataset) {
    vector<pair<int, float>> brute_res;

    for (size_t i = 0; i < dataset.size(); i++) {
        if (q_idx == i) continue;
        float d = euclideanDist(q, dataset[i], ivfpq->image_size);
        brute_res.push_back({(int)i, d});
    }

    sort(brute_res.begin(), brute_res.end(), comparePairs);
    if (brute_res.size() > ivfpq->n)
        brute_res.resize(ivfpq->n);
    return brute_res;
}