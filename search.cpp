#include <iostream>
#include <cstring>
#include <cstdio>

#include "parameters.hpp"

using namespace std;

void swapEndian(u_int32_t* value) {
    u_int32_t byte4 = (*value >> 24) & 0x000000FF;
    u_int32_t byte3 = (*value >> 8) & 0x0000FF00;
    u_int32_t byte2 = (*value << 8) & 0x00FF0000;
    u_int32_t byte1 = (*value << 24) & 0xFF000000;
    *value =  byte1 | byte2| byte3 | byte4;
}

void readInputMnist() {
    FILE* fd = fopen("input.dat", "r");
    if (fd == NULL) {
            perror("Failed to open file");
        exit(errno);
    }
    u_int32_t magic_num, num_of_images, num_of_rows, num_of_columns;
    fread(&magic_num, sizeof(u_int32_t), 1, fd);
    fread(&num_of_images, sizeof(u_int32_t), 1, fd);
    fread(&num_of_rows, sizeof(u_int32_t), 1, fd);
    fread(&num_of_columns, sizeof(u_int32_t), 1, fd);

    swapEndian(&magic_num);
    swapEndian(&num_of_images);
    swapEndian(&num_of_rows);
    swapEndian(&num_of_columns);
    cout << "magic number: " << magic_num << "\nnumber of images: " << num_of_images << "\nnumber of rows: " << num_of_rows << "\nnumber of columns:" << num_of_columns << endl;
    return;
}


int main(int argc, char* argv[]){
    Params* p = ArgsParser(argc, argv);
    initializeParams(p);
    printParameters(p);

    if(p->type == "mnist")
        readInputMnist();

    delete(p);
    return 0;
}