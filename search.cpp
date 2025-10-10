#include <iostream>
#include <cstring>
#include <cstdio>

#include "parameters.hpp"

using namespace std;


int main(int argc, char* argv[]){
    Params* params = ArgsParser(argc, argv);
    initializeParams(params);
    cout << params->k << endl;

    delete(params);
    return 0;
}