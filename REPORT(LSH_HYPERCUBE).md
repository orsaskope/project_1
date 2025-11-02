# Performance Comparison Report

This report summarizes the performance of **LSH** and **Hypercube** on the **MNIST** and **SIFT** datasets.  

Metrics shown:  
- **AF** = Approximation Factor (closer to 1 is better)  
- **Recall@N** = % of true neighbors found  
- **QPS** = Queries per second (higher is faster)  
- **tApproximate** = time of approximate search  
- **tTrue** = time of exact brute-force KNN  

**IMPORTANT NOTES:**

- A subset of **100 query vectors** was used in all experiments for consistency and runtime efficiency.
- **MNIST:** 60,000 vectors for input, 10,000 for query.
- **SIFT:** 1,000,000 vectors for input, 10,000 for query.

---

## 1. LSH on MNIST

| w | L | k | Average AF | Recall@N | QPS | tApproximate | tTrue |
|---|---|---|-----------:|---------:|----:|-------------:|------:|
4 | 5 | 4 | 1.243086 | 0.090000 | 848.858193 | 0.001178 | 0.239391
4 | 5 | 2 | 1.036229 | 0.550000 | 31.310004 | 0.031939 | 0.250938
4 | 5 | 3 | 1.112232 | 0.230000 | 157.603800 | 0.006345 | 0.245269
4 | 8 | 3 | 1.085452 | 0.290000 | 103.797215 | 0.009634 | 0.250607
4 | 15 | 3 | 1.058408 | 0.470000 | 53.786093 | 0.018592 | 0.245782
4 | 15 | 2 | 1.006404 | 0.860000 | 11.210820 | 0.089200 | 0.245247

---

## 2. Hypercube on MNIST

| kproj | M | probes | w | Average AF | Recall@N | QPS | tApproximate | tTrue |
|------:|--:|-------:|--:|-----------:|---------:|----:|-------------:|------:|
14 | 10 | 2 | 4 | 1.622202 | 0.020000 | 7883.767410 | 0.000127 | 0.249606
14 | 20 | 2 | 4 | 1.476958 | 0.050000 | 6056.860718 | 0.000165 | 0.248907
14 | 200 | 15 | 4 | 1.191826 | 0.160000 | 1042.043583 | 0.000960 | 0.253344
14 | 500 | 25 | 5 | 1.124167 | 0.290000 | 426.387929 | 0.002345 | 0.253667
14 | 2000 | 50 | 6 | 1.067394 | 0.520000 | 115.344545 | 0.008670 | 0.251743
14 | 1000 | 50 | 6 | 1.092292 | 0.380000 | 215.511740 | 0.004640 | 0.247729
14 | 1000 | 25 | 5 | 1.080929 | 0.450000 | 233.696145 | 0.004279 | 0.250098
14 | 3000 | 100 | 6 | 1.044214 | 0.580000 | 74.551496 | 0.013414 | 0.249505
18 | 6000 | 200 | 6 | 1.038541 | 0.640000 | 76.691642 | 0.013039 | 0.258848
18 | 6000 | 300 | 6 | 1.030866 | 0.700000 | 61.852808 | 0.016167 | 0.238871

---

## 3. LSH on SIFT 

| w | L | k | Average AF | Recall@N | QPS | tApproximate | tTrue |
|---|---|---|-----------:|---------:|----:|-------------:|------:|
4 | 5 | 4 | 2.251180 | 0.000000 | 9424.454235 | 0.000106 | 1.257109
30 | 10 | 2 | 1.321071 | 0.090000 | 105.021959 | 0.009522 | 1.240682
50 | 10 | 2 | 1.244735 | 0.150000 | 39.853898 | 0.025092 | 1.244487
100 | 10 | 3 | 1.204551 | 0.210000 | 112.833814 | 0.008863 | 1.242618
100 | 10 | 2 | 1.090888 | 0.380000 | 11.004312 | 0.090873 | 1.240978
200 | 15 | 3 | 1.026586 | 0.650000 | 10.942237 | 0.091389 | 1.237124
200 | 15 | 2 | 1.002420 | 0.940000 | 2.135077 | 0.468367 | 1.227476

---

## 4. Hypercube on SIFT 

| kproj | M | probes | w | Average AF | Recall@N | QPS | tApproximate | tTrue |
|------:|--:|-------:|--:|-----------:|---------:|----:|-------------:|------:|
14 | 10 | 2 | 4 | 1.749344 | 0.080000 | 5688.482757 | 0.000176 | 1.292824
14 | 300 | 20 | 150 | 1.330352 | 0.120000 | 990.911214 | 0.001009 | 1.317695
14 | 500 | 40 | 150 | 1.298942 | 0.140000 | 876.003761 | 0.001142 | 1.314671
16 | 1000 | 100 | 150 | 1.200068 | 0.190000 | 387.022161 | 0.002584 | 1.284559
18 | 3000 | 300 | 150 | 1.113823 | 0.330000 | 112.514129 | 0.008888 | 1.295206
18 | 30000 | 3000 | 150 | 1.024807 | 0.680000 | 20.316088 | 0.049222 | 1.294991
18 | 50000 | 3000 | 150 | 1.018271 | 0.750000 | 11.406940 | 0.087666 | 1.298279

---

## 5. Conclusions

### MNIST
- **LSH** performs well when the number of hash tables (**L**) is increased or when fewer hash functions (**k**) are concatenated.  
  - Lower **k** increases recall but slows down query time.  
  - Higher **k** speeds up search but reduces accuracy.
- **Hypercube** shows strong speed (very high QPS), especially with low `M` and `probes`, but initially suffers in recall and AF.
- Increasing **M** and **probes** in the Hypercube gradually improves recall and AF, but at the cost of longer search time.
- **Best trade-off:** Both methods can achieve **Recall@N ~ 0.6–0.7** when tuned, but LSH tends to be simpler and more stable, while Hypercube requires more careful parameter balancing.

### SIFT
- SIFT vectors are higher dimensional and more complex, so the parameter effect is more pronounced.
- **LSH**:
  - For small `w` and high `k`, LSH becomes extremely fast but loses almost all accuracy.
  - Increasing `w` and adjusting `L` and `k` leads to significantly improved recall (up to **~0.94**), but query time increases.
- **Hypercube**:
  - Shows a **very clear smooth accuracy-speed trade-off**.
  - Small `M`/`probes` → extremely fast but low recall.
  - Large `M`/`probes` → slower but very high recall (**up to ~0.75**) and AF close to 1.
- **Best trade-off:** Hypercube achieved **better accuracy than LSH** on SIFT at similar or slightly slower QPS, showing that it scales more effectively for high-dimensional real-valued data.

### Overall Key Insights
- **LSH is easier to tune** and while slightly slower can achieve greater recall on data like MNIST.
- **Hypercube provides better control** over accuracy-speed trade-offs and tends to perform **better on high-dimensional datasets** like SIFT when properly tuned.
- **Choosing parameters is crucial**:
  - LSH: Balance `L` (tables) and `k` (hashes per table).
  - Hypercube: Balance `M` (candidates) and `probes`.

### Practical Recommendation
- For **MNIST**:  
  → **LSH** can bring you more accuracy but at much slower speed.
  
  → **Hypercube** brings you more accuracy but less speed.

- For **SIFT**:  
  → **Hypercube** generally yields **higher recall** and **better approximation quality** when allowed moderate query time.

### IMPORTANT , FOR THE SCORES ABOVE ,SEED=1,N=1 WAS USED
