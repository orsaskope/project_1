#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>

#include "parameters.hpp"

using namespace std;

// In case of MNIST input, we need to swap big->little endian for the first values
void swapEndian(u_int32_t* value) {
    u_int32_t byte4 = (*value >> 24) & 0x000000FF;
    u_int32_t byte3 = (*value >> 8) & 0x0000FF00;
    u_int32_t byte2 = (*value << 8) & 0x00FF0000;
    u_int32_t byte1 = (*value << 24) & 0xFF000000;
    *value =  byte1 | byte2| byte3 | byte4;
}

// Function to read MNIST input file
vector<vector<unsigned char>> readInputMnist(const char* file) {
    FILE* fd = fopen(file, "r");
    if (fd == NULL) {
            perror("Failed to open file");
        exit(errno);
    }
    u_int32_t magic_num, images, rows, columns;
    fread(&magic_num, sizeof(u_int32_t), 1, fd);
    fread(&images, sizeof(u_int32_t), 1, fd);
    fread(&rows, sizeof(u_int32_t), 1, fd);
    fread(&columns, sizeof(u_int32_t), 1, fd);

    swapEndian(&magic_num);
    swapEndian(&images);
    swapEndian(&rows);
    swapEndian(&columns);
    cout << "magic number: " << magic_num << "\nnumber of images: " << images << "\nnumber of rows: " << rows << "\nnumber of columns:" << columns << endl;
    
    // We create a vector of vectors to store all the images.
    // Each element (dataset[i]) is a vector<unsigned char> of size 'size' that contains an image.
    // No need to swap big to little endian since we are reading single bytes.
    int size = rows * columns;
    vector<vector<unsigned char>> dataset(images, vector<unsigned char>(size));
    for (int i = 0; i < images; i++) {
        int res = fread(dataset[i].data(), sizeof(unsigned char), size, fd);
        if (res != size) {
            cout << "error in reading mnist images of size " << size << endl;
            exit(errno);
        }
    }
    return dataset;
}


int main(int argc, char* argv[]){
    Params* p = ArgsParser(argc, argv);
    initializeParams(p);
    printParameters(p);
    
    vector<vector<unsigned char>> dataset;
    if(p->type == "mnist")
        dataset = readInputMnist(p->input.c_str());
    for (int i = 0; i < dataset[0].size(); i++) {
       printf("%u ",dataset[0][i]);
    }

    delete(p);
    return 0;
}