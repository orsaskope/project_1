#include "parameters.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib> 

Params::Params() {
    input = nullptr;
    query = nullptr;
    o = nullptr;
    k = 0;
    l = 0;
    w = 0.0;
    n = 0;
    r = 0;
    type = "";
    range = false;

    kproj = 0;
    m = 0;
    probes = 0;

    kclusters = 0;
    nprobe = 0;
    seed = 0;
    nbits = 0;
    algorithm = -1;
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

    if (args->algorithm == -1) {
        cout << "Please choose an algorithm!" << endl;
        exit (1);
    }
    return args;
}

void initializeParams(Params* params) {
    // Initialize parameters with the default values given
    if (params->seed == 0) params->seed = 1;
    if (params->n == 0) params->n = 1;
    if (params->r == 0) {
        if (params->type == "mnist") params->r = 2000;
        else params->r = 2;
    }

    if (params->algorithm == 0) {    // LSH
        if (params->k == 0) params->k = 4;
        if (params->l == 0) params->l = 5;
        if (params->w == 0.0) params->w = 4.0;
        return;
    }
    if (params->algorithm == 1) {    // Hypercube
        if (params->kproj == 0) params->kproj = 14;
        if (params->w == 0.0) params->w = 4.0;
        if (params->m == 0) params->m = 10;
        if (params->probes == 0) params->probes = 2;
        return;
    }
    if (params->algorithm == 2) {    // IVFFLAT
        if (params->kclusters == 0) params->kclusters = 50;
        if (params->nprobe == 0) params->nprobe = 5;
        return;
    }
    if (params->algorithm == 3) {    // IVFPQ
        if (params->kclusters == 0) params->kclusters = 50;
        if (params->nprobe == 0) params->nprobe = 5;
        if (params->nbits == 0) params->nbits = 8;
        if (params->m == 0) params->m = 16;
        return;
    }
}
