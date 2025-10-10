#include <iostream>
#include <cstring>
using namespace std;

struct Params{
    FILE* input;    // Input file
    FILE* query;    // Output file
    int k;          // Number of LSH functions for g
    int l;          // Number of hashtables
    double w;       // Cell size on the straight line
    FILE* o;        // Output file
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
};

void validArgument(char* argument, int argc, int i) {
    if(i >= argc) {
        cout << "Not enough values given" << endl;
        exit (1);
    }
    if (argument[0] == '-') {
        cout << "Invalid argument: " << argument << endl;
        exit(1);
    }
}

Params* ArgsParser(int argc, char* argv[]) {
    Params* args = new Params();
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-d") cout << "-d" << endl;
        else if (arg == "-q") cout << "-q" << endl;
        else if (arg == "-o") cout << "-o" << endl;

        else if (arg == "-k") {
            validArgument(argv[i+1], argc, i+1);
            args->k = stoi(argv[++i]);
        }
        else if (arg == "-L") {
            validArgument(argv[i+1], argc, i+1);
            args->l = stoi(argv[++i]);
        }
        else if (arg == "-w") {
            validArgument(argv[i+1], argc, i+1);
            args->w = stod(argv[++i]);
        }
        else if (arg == "-N") {
            validArgument(argv[i+1], argc, i+1);
            args->n = stoi(argv[++i]);
        }
        else if (arg == "-R") {
            validArgument(argv[i+1], argc, i+1);
            args->r = stoi(argv[++i]);
        }
        else if (arg == "-kproj") {
            validArgument(argv[i+1], argc, i+1);
            args->kproj = stoi(argv[++i]);
        }
        else if (arg == "-M") {
            validArgument(argv[i+1], argc, i+1);
            args->m = stoi(argv[++i]);
        }
        else if (arg == "-probes") {
            validArgument(argv[i+1], argc, i+1);
            args->probes = stoi(argv[++i]);
        }
        else if (arg == "-kclusters") {
            validArgument(argv[i+1], argc, i+1);
            args->kclusters = stoi(argv[++i]);
        }
        else if (arg == "-nprobe") {
            validArgument(argv[i+1], argc, i+1);
            args->nprobe = stoi(argv[++i]);
        }
        else if (arg == "-nbits") {
            validArgument(argv[i+1], argc, i+1);
            args->nbits = stoi(argv[++i]);
        }
        
        else if (arg == "-type") {
            validArgument(argv[i+1], argc, i+1);
            i++;
            if (strcmp(argv[i], "sift") && strcmp(argv[i], "mnist")) {
                cout << "Invalid '-type' parameter!" << endl;
                cout << "a" << argv[i] << "a" << endl;
                exit(1);
            }
            args->type = argv[i];
        }
        else if (arg == "-range") {
            validArgument(argv[i+1], argc, i+1);
            i++;
            if (!strcmp(argv[i], "true") || !strcmp(argv[i], "t")) args->range = true;
            else if (!strcmp(argv[i], "false") || !strcmp(argv[i], "f")) args->range = false;
            else {
                cout << "Invalid '-range' parameter" << endl;
                cout << "a" << argv[i] << "a" << endl;
                exit(1);
            }
        }
        else if (arg == "-lsh") args->algorithm = 0;
        else if (arg == "-hypercube") args->algorithm = 1;
        else if (arg == "-ivfflat") args->algorithm = 2;
        else if (arg == "-ivfpq") args->algorithm = 3;
    }
    return args;
} 

int main(int argc, char* argv[]){
    Params* params = ArgsParser(argc, argv);
    cout << params->k << endl;

    free(params);
    return 0;
}