#include "LSH.hpp"

LSH::LSH(int L_, int k_, int dim_, double w_, unsigned seed_)
    : L(L_), k(k_), dim(dim_), w(w_), seed(seed_), rng(seed_)
{
    std::normal_distribution<double> nd(0.0, 1.0);
    std::uniform_real_distribution<double> ud(0.0, w);
    std::cout << "LSH constructor has been used \n";
    a.resize(L, std::vector<std::vector<double>>(k, std::vector<double>(dim)));
    b.resize(L, std::vector<double>(k, 0.0));
    hashTables.resize(L);

    for (int l = 0; l < L; ++l) {
        for (int i = 0; i < k; ++i) {
            for (int d = 0; d < dim; ++d) {
                a[l][i][d] = nd(rng);
            }
            b[l][i] = ud(rng);
        }
    }

    r.resize(L, std::vector<long long>(k));
    std::uniform_int_distribution<long long> ri_dist(1, 1e9); // random integers

    for (int l = 0; l < L; ++l) {
        for (int i = 0; i < k; ++i) {
            r[l][i] = ri_dist(rng);
        }
    }

}

std::vector<long long> LSH::compute_h(const std::vector<double>& point, int tableIdx) {
    std::vector<long long> hvals(k);

    for (int i = 0; i < k; ++i) {
        double dot = 0.0;
        for (int d = 0; d < dim; ++d)
            dot += a[tableIdx][i][d] * point[d];

        double val = (dot + b[tableIdx][i]) / w;
        hvals[i] = static_cast<long long>(std::floor(val));
    }

    return hvals;
}

LSH::GResult LSH::compute_g(const std::vector<double>& point, int tableIdx, int n ,int index) {
    static const unsigned long long M = 4294967291ULL;
    auto hvals = compute_h(point, tableIdx );

    // (a+b) mod M =((a mod M)+(b mod M)) mod M therefore
    // sum=(r1​h1​+r2​h2​+…+rk​hk​) mod M is equivalent to sum=(((r1​h1​) mod M)+ ((r2​h2​) mod M)+ ..... + ((rk​hk​) mod M)) mod M which is better because 
    // that way we get to also search for the case of overflow.
    // Doing sum %= M at every step: 
    // 1)Keeps the intermediate sum from overflowing
    // 2)Preserves the correct modular arithmetic result


    unsigned long long sum = 0;
    for (int i = 0; i < k; ++i) {
        sum += (r[tableIdx][i] * static_cast<unsigned long long>(hvals[i])) % M;
        sum %= M;
    }

    unsigned long long TableSize = n / 4;  // or n/8, n/16
    unsigned long long ID = sum % M;       // the locality-sensitive ID
    unsigned long long g_val = ID % TableSize;

    return { index,std::to_string(g_val), ID };
}

void LSH::insert(int index, const std::vector<double>& point, int n) {
    for (int l = 0; l < L; ++l) {
        GResult g_res = compute_g(point, l, n , index);
        hashTables[l][g_res.g_key].push_back(g_res);

    }
}



void LSH::print_tables() {
    for (int l = 0; l < L; ++l) {
        std::cout << "Table " << l << " has " << hashTables[l].size() << " buckets\n";
    }
}


