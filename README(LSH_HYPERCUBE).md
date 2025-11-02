# LSH – Υλοποιηση

Για την υλοποιηση της LSH χρησιμοποιουνται οι εξης συναρτησεις:

## `run_mnist_experiment_lsh`  (Params* p, MNISTData& mnist)

Αυτή η συνάρτηση τρέχει ολόκληρο το πείραμα LSH πάνω στο MNIST dataset.  
Παίρνει τις παραμέτρους (Params* p, MNISTData& mnist), φορτώνει τα δεδομένα, βρισκει τους Ν κοντινότερους γειτονες και μετά συγκρίνει τα αποτελέσματα του LSH με brute force για να μετρήσει απόδοση & ποιότητα.

---

## Τι κάνει βήμα-βήμα

### 1. Φορτώνει το MNIST dataset (Input)
- Διαβάζει τις εικόνες (τις μετατρέπει σε `double` για ευκολία).

### 2. Κανονικοποιεί τα δεδομένα input (Normalization)
- Μετατρέπει όλα τα pixel από `0–255` → `0–1` για σταθερότερες αποστάσεις.

### 3. Δημιουργεί ένα αντικείμενο LSH
- `L` hash tables
- `k` hash functions ανά table
- `w` ως bucket width
- `seed` για αναπαραγωγιμότητα

### 4. Φορτώνει τα δεδομένα στο LSH (insert)
- Κάνει `insert` όλα τα images του dataset στους πίνακες.



### Για κάθε **query vector**:

- **Φόρτωση query (MNIST Query vector)**
  - Διαβάζει το query και το μετατρέπει σε `double`.

- **Κανονικοποίηση query**
  - Μετατρέπει τα pixels από `0–255` → `0–1`.

- **LSH k-NN + timing**
  - Τρέχει `lsh->knn_search_mnist(...)` για να βρει τους **N** κοντινότερους.
  - Μετρά τον χρόνο εκτέλεσης (tApproximate, QPS).

- **Brute Force k-NN + timing**
  - Τρέχει brute force για τους **N** κοντινότερους.
  - Μετρά τον χρόνο εκτέλεσης (tTrue).

- **Σύγκριση αποτελεσμάτων**
  - Υπολογίζει **AF (Approximation Factor)**.
  - Υπολογίζει **Recall@N**.
  - Γράφει αναλυτικά τα αποτελέσματα (IDs/αποστάσεις) στο αρχείο.

- **Προαιρετικό Range Search**
  - Αν είναι ενεργό (`p->range`), εκτελεί `range_search_mnist`.
  - Τυπώνει τα IDs που πέφτουν μέσα στην ακτίνα `R`.

---

## `run_sift_experiment_lsh` (Params* p, SIFTData& sift)

Τρέχει το πείραμα LSH πάνω στο **SIFT** dataset.  
Παίρνει τις παραμέτρους (`Params* p, SIFTData& sift`), φορτώνει τα δεδομένα, βρίσκει τους **N** κοντινότερους γείτονες με LSH και συγκρίνει τα αποτελέσματα με **brute force** για να μετρήσει απόδοση & ποιότητα.

---

## Τι κάνει βήμα-βήμα

### 1. Φορτώνει το SIFT dataset (Input)
- Διαβάζει τα SIFT descriptors (float vectors).
- **Δεν** τα κανονικοποιεί (δουλεύει raw) αλλα δημιουργουμε το dataset και σε double.

### 2. Δημιουργεί ένα αντικείμενο LSH
- `L` hash tables  
- `k` hash functions ανά table  
- `w` ως bucket width  
- `seed` για αναπαραγωγιμότητα

### 3. Φορτώνει τα δεδομένα στο LSH (insert)
- Κάνει `insert` όλα τα SIFT vectors στους πίνακες.

---

### Για κάθε **query vector**:

- **Φόρτωση query (SIFT Query vector)**
  - Διαβάζει το query ως float vector.
  - To μετατρεπει σε double.

- **LSH k-NN + timing**
  - Τρέχει `lsh->knn_search(...)` (SIFT έκδοση) για να βρει τους **N** κοντινότερους.
  - Μετρά τον χρόνο εκτέλεσης (tApproximate, QPS).

- **Brute Force k-NN + timing**
  - Τρέχει `brute_force_knn_sift(...)` για τους **N** κοντινότερους.
  - Μετρά τον χρόνο εκτέλεσης (tTrue).

- **Σύγκριση αποτελεσμάτων**
  - Υπολογίζει **AF (Approximation Factor)**.
  - Υπολογίζει **Recall@N**.
  - Γράφει αναλυτικά τα αποτελέσματα (IDs/αποστάσεις) στο αρχείο.

- **Προαιρετικό Range Search**
  - Αν είναι ενεργό (`p->range`), εκτελεί `range_search_sift`.
  - Τυπώνει τα IDs που πέφτουν μέσα στην ακτίνα `R` (χωρίς normalization).

---

## Σημειώσεις
- Τα SIFT vectors δουλεύουν **ως έχουν** (raw). Το `radius` στο range search είναι στην ίδια κλίμακα με τα SIFT descriptors.

- **Καλές τιμές R για lsh:**
  - Sift:250
  - Mnist:1020
  
---
## Παρακάτω υπάρχει ανάλυση των συναρτήσεων που χρησιμοποιούν οι implemtation συναρτησεις της lsh.
---

### `LSH::LSH(int L_, int k_, int dim_, double w_, unsigned seed_)`
Φτιάχνει το LSH αντικείμενο.  
Δημιουργεί `L` hash tables, παράγει τυχαία διανύσματα προβολής `a` και μετατοπίσεις `b`,ορίζει τυχαίους συντελεστές `r` για το hashing και δημιουργεί τυχαιότητα στους αριθμούς με την rng(seed_).


---

### `compute_h(const std::vector<double>& point, int tableIdx)`
Υπολογίζει τα **k** μικρά hash values (`h1, h2, ..., hk`) για ένα σημείο.  
Κάνει dot product με τα τυχαία διανύσματα και χρησιμοποιεί floor((dot + b) / w) για να δημιουργήσει τα hash values.

---

### `compute_g(...)`
Συνδυάζει τα `h` values σε **ένα** τελικό hash key που χρησιμοποιούμε για να μπούμε σε bucket.  
Επιστρέφει struct με:  
- index του σημείου  
- το string key του bucket  
- και το τελικό hash ID.

---

### `insert(int index, const std::vector<double>& point, int n)`
Προσθέτει ένα σημείο **σε όλα τα L hash tables** στο αντίστοιχο bucket.  
Προτού αρχίσουμε να κάνουμε queries, πρέπει να έχουμε “φορτώσει” όλα τα στοιχεία του input με αυτό.

---

### `print_tables()`
Απλά δείχνει πόσα buckets έχει κάθε table.  
Χρήσιμο για να καταλάβεις αν η κατανομή γίνεται σωστά.

---

### `get_candidates(const std::vector<double>& query, int n)`
Βρίσκει τους **υποψήφιους γείτονες** του query.  
Ψάχνει μόνο στα buckets όπου το query πέφτει — άρα δεν τσεκάρουμε όλα τα σημεία (Αποφεύγει duplicates).

---

### `knn_search(...)` (για SIFT)
Παίρνει candidates , υπολογίζει αποστάσεις , ταξινομεί τους γείτονες και επιστρέφει τους **k** κοντινότερους.  
Dataset είναι `float` εδώ.

---

### `knn_search_mnist(...)` (για MNIST)
Ίδια διαδικασία με `knn_search(...)` (για SIFT), αλλά dataset είναι `double`.

---

### `range_search_mnist(...)`
Ψάχνει **όλα** τα σημεία που είναι μέσα σε μία ακτίνα από το query.  
Για MNIST η ακτίνα κανονικοποιείται (`radius / 255.0`) επειδή τα pixels είναι normalized.

(Μια καλή τιμή για testing ειναι R = 1020 καθως θα γινει R(normalized) = 4 που ειναι μια καλη τιμη για να τεσταρει την λειτουργια της range search σε normalized mnist δεδομενα.)

---

### `range_search_sift(...)`
Όμοιο με το MNIST range search, απλά εδώ **δεν** κανονικοποιείται η ακτίνα.  
Δουλεύει απευθείας με τα raw SIFT vectors.

(Μια καλή τιμή για testing ειναι R = 250 για να τεσταρει την λειτουργια της range search σε raw SIFT δεδομενα.)

---





# Hypercube – Υλοποίηση

Για την υλοποίηση του **Hypercube** χρησιμοποιούνται οι εξής συναρτήσεις:

---

## `run_mnist_experiment_hypercube` (Params* p, MNISTData& mnist)

Τρέχει ολόκληρο το πείραμα **Hypercube** πάνω στο **MNIST**.  
Παίρνει τις παραμέτρους (`Params* p, MNISTData& mnist`), με χρηση hypercube βρίσκει τους **N** κοντινότερους γείτονες και συγκρίνει με **brute force** για ποιότητα & ταχύτητα.

---

## Τι κάνει βήμα-βήμα

### 1. Φορτώνει το MNIST dataset (Input)
- Διαβάζει τις εικόνες και τις μετατρέπει σε `double`.

### 2. Κανονικοποιεί τα δεδομένα input (Normalization)
- Pixels `0–255` → `0–1` (σταθερότερες αποστάσεις).

### 3. Δημιουργεί Hypercube
- `kproj`: πλήθος προβολών/bit του vertex key  
- `w`: bucket width  
- `seed`: αναπαραγωγιμότητα  
- `M`: μέγιστος αριθμός υποψηφίων προς συλλογή  
- `probes`: πόσα διαφορετικά vertices θα εξερευνήσει (με flips)

### 4. Φορτώνει τα δεδομένα στο Hypercube (insert)
- Κάνει `insert` όλα τα vectors του dataset στα κατάλληλα vertices.

---

### Για κάθε **query vector**:

- **Φόρτωση & κανονικοποίηση**
  - Διαβάζει query, το κάνει `double` και το κανονικοποιεί `0–1`.

- **Hypercube αναζήτηση + timing**
  - `cube->query(query_vec, p->m)` για συλλογή υποψηφίων (μέχρι `M`).
  - `cube->find_top_n_neighbors(...)` για τους **N** πιο κοντινούς.
  - Μετρά χρόνο (tApproximate/QPS).

- **Brute Force + timing**
  - `brute_force_knn(...)` για ακριβή σύγκριση.
  - Μετρά χρόνο (tTrue).

- **Σύγκριση αποτελεσμάτων**
  - Υπολογίζει **AF (Approximation Factor)** και **Recall@N**.
  - Γράφει αναλυτικά αποτελέσματα (IDs/αποστάσεις) στο αρχείο.

- **Προαιρετικό Range Search**
  - Αν `p->range`, τρέχει `cube->range_search(...)`.
  - Τυπώνει IDs εντός ακτίνας `R` (με **normalized** R: `R/255.0`).

---

## `run_sift_experiment_hypercube` (Params* p, SIFTData& sift)

Τρέχει Hypercube πάνω σε **SIFT** descriptors.  
Παίρνει (`Params* p, SIFTData& sift`), φτιάχνει index, βρίσκει **N** κοντινότερους και συγκρίνει με **brute force**.

> Κύριες διαφορές vs MNIST:
> - Χρησιμοποιεί **SIFT data** (float → μετατροπή σε `double` για υπολογισμούς).
> - **Δεν** γίνεται normalization (δουλεύει raw).
> - Range search με **raw radius**.

---

## Τι κάνει βήμα-βήμα

### 1. Φορτώνει το SIFT dataset (Input)
- Διαβάζει SIFT vectors (float), τα αποθηκεύει ως `double` (χωρίς normalization).

### 2. Δημιουργεί Hypercube
- `kproj`, `w`, `seed`, `M`, `probes` όπως παραπάνω.

### 3. Φορτώνει τα δεδομένα στο Hypercube (insert)
- Κάνει `insert` όλα τα SIFT vectors στους vertices.

---

### Για κάθε **query vector**:

- **Φόρτωση query**
  - Διαβάζει query (ως `double`), **χωρίς** normalization.

- **Hypercube αναζήτηση + timing**
  - `query(...)` → συλλογή υποψηφίων (έως `M`).
  - `find_top_n_neighbors(...)` → **N** πιο κοντινοί.
  - Χρόνος (tApproximate/QPS).

- **Brute Force + timing**
  - `brute_force_knn_sift(...)` (raw SIFT) για αλήθεια.
  - Χρόνος (tTrue).

- **Σύγκριση**
  - **AF**, **Recall@N**, αναλυτικά αποτελέσματα.

- **Προαιρετικό Range Search**
  - Αν `p->range`, `range_search_sift(...)`.
  - IDs εντός ακτίνας `R` (**raw**).

---

## Σημειώσεις
- **MNIST**: normalized αποστάσεις → range search με `R/255.0`.  
- **SIFT**: raw αποστάσεις → range search με raw `R`.  
- Οι παράμετροι **`M`** (max candidates) και **`probes`** (πόσα vertices θα εξερευνήσεις) ρυθμίζουν trade-off **ταχύτητας/ποιότητας**.
- Ίδιο `seed` + ίδια σειρά εισαγωγών/queries → αναπαραγωγιμότητα.

---

## Παρακάτω υπάρχει ανάλυση των συναρτήσεων που χρησιμοποιούν οι implementation συναρτήσεις του hypercube.

### `Hypercube::Hypercube(int kproj_, int dim_, double w_, unsigned seed_, int M_, int probes_)`
- Ορίζει προβολές `a` (Gaussian), μετατοπίσεις `b` (Uniform `[0,w)`), και RNG με `seed`.  
- Αποθηκεύει `kproj` bits ανά σημείο (vertex key), καθώς και `M`/`probes` όρια για την αναζήτηση.

---

### `compute_vertex(const std::vector<double>& point, int index)`
- Υπολογίζει το **binary key** του σημείου: για κάθε προβολή ελέγχει το πρόσημο του \((a·x + b)/w\) και γράφει `0/1`.  
- Επιστρέφει `{ index, vertex_key }`.

---

### `insert(int index, const std::vector<double>& point)`
- Υπολογίζει το `vertex_key` και βάζει το σημείο στο `hypercubeTable[vertex_key]`.

---

### `print_table_info()`
- Εκτυπώνει πλήθος κορυφών του πίνακα και πόσα σημεία έχει κάθε vertex.

---

### `query(const std::vector<double>& point, int M)`
- Βρίσκει υποψήφιους από:
  1) το **ίδιο vertex** με το query και  
  2) γειτονικά vertices με **BFS flip 1-bit** τη φορά, μέχρι να φτάσει **`probes`** ή να μαζέψει **`M`** υποψήφιους.  
- Επιστρέφει λίστα υποψηφίων `VertexResult`.

---

### `find_top_n_neighbors(query, candidates, N, dataset)`
- Υπολογίζει ευκλείδειες αποστάσεις για τους `candidates`,  
- Ταξινομεί και επιστρέφει τους **N** κοντινότερους `VertexResult`.

---

### `range_search(query_point, radius, M, dataset)` (MNIST)
- Παίρνει υποψήφιους με `query(...)`.  
- Ελέγχει ποιοι έχουν απόσταση ≤ `radius/255.0` (normalized MNIST).  
- Επιστρέφει όσους είναι εντός ακτίνας.

---

### `range_search_sift(query_point, radius, M, dataset)` (SIFT)
- Παίρνει υποψήφιους με `query(...)`.  
- Ελέγχει ποιοι έχουν απόσταση ≤ `radius` (raw SIFT).  
- Επιστρέφει όσους είναι εντός ακτίνας.

---
