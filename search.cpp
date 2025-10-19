#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <cerrno>

#include "parameters.hpp"
#include "LSH.hpp"
#include "ivfflat.hpp"

using namespace std;

void swapEndian(u_int32_t* value) {
    u_int32_t byte4 = (*value >> 24) & 0x000000FF;
    u_int32_t byte3 = (*value >> 8) & 0x0000FF00;
    u_int32_t byte2 = (*value << 8) & 0x00FF0000;
    u_int32_t byte1 = (*value << 24) & 0xFF000000;
    *value =  byte1 | byte2| byte3 | byte4;
}


MNISTData readInputMnist(FILE* fd) {
    u_int32_t magic_num, images, rows, columns;
    fread(&magic_num, sizeof(u_int32_t), 1, fd);
    fread(&images, sizeof(u_int32_t), 1, fd);
    fread(&rows, sizeof(u_int32_t), 1, fd);
    fread(&columns, sizeof(u_int32_t), 1, fd);

    swapEndian(&magic_num);
    swapEndian(&images);
    swapEndian(&rows);
    swapEndian(&columns);

    cout << "magic number: " << magic_num << "\nnumber of images: " << images
         << "\nnumber of rows: " << rows << "\nnumber of columns: " << columns << endl;

    int size = rows * columns;

    MNISTData data;
    data.magic_number = magic_num;
    data.number_of_images = images;
    data.n_rows = rows;
    data.n_cols = columns;
    data.image_size = size;
    data.images.resize(images, std::vector<unsigned char>(size));

    int i_images = static_cast<int>(images);
    for(int i = 0; i < i_images; i++) {
        int res = fread(data.images[i].data(), sizeof(unsigned char), size, fd);
        if (res != size) {
            cout << "error in reading mnist images of size " << size << endl;
            exit(errno);
        }
    }

    return data;
}

vector<vector<float>> readInputSift(FILE* fd) {
    vector<vector<float>> dataset(1000000, vector<float>(128));
    int32_t dim;

    for (int i = 0; i < 1000000; i++) {
        int res = fread(&dim, sizeof(dim), 1, fd);
        res = fread(dataset[i].data(), sizeof(float), 128, fd);
        if (res != 128) {
            cout << "error in reading sift vectors" << endl;
            exit(errno);
        }
    }
    return dataset;
}

int main(int argc, char* argv[]) {
    Params* p = ArgsParser(argc, argv);
    initializeParams(p);
    printParameters(p);

    LSH* lsh = nullptr;
    IVFFLAT* ivfflat = nullptr;

    FILE* fd = fopen(p->input.c_str(), "r");
    if (fd == NULL) {
        perror("Failed to open file");
        exit(errno);
    }

    if (p->type == "mnist") {
        MNISTData mnist = readInputMnist(fd);
        
        std::cout << "MNIST dataset loaded with " << mnist.number_of_images << " images.\n";
        
        if (p->algorithm == 0) {
            lsh = new LSH(p->l, p->k, mnist.image_size, p->w, p->seed);

            for (int i = 0; i < mnist.number_of_images; ++i) {
                std::vector<double> image_double(mnist.image_size);
                for (int j = 0; j < mnist.image_size; ++j) {
                    image_double[j] = static_cast<double>(mnist.images[i][j]) / 255.0;
                }

                lsh->insert(i, image_double ,mnist.number_of_images);
            }

            lsh->print_tables();
            return 0;
        }
        if (p->algorithm == 2) {
            ivfflat = new IVFFLAT(p->seed, p->kclusters, p->nprobe, p->n, p->r);

            vector<vector<float>> image_float(mnist.number_of_images, vector<float>(mnist.image_size));
            for (int i = 0; i < mnist.number_of_images; ++i) {
                for (int j = 0; j < mnist.image_size; ++j) {
                    image_float[i][j] = static_cast<float>(mnist.images[i][j]) / 255.0;
                }

                IvfflatSearch(image_float, ivfflat);
            }

        }



    } else {
        vector<vector<float>> dataset = readInputSift(fd);
        std::cout << "SIFT dataset loaded.\n";
        lsh = new LSH(p->l, p->k, 128, p->w, p->seed);
    }


    fclose(fd);
    delete(p);
    return 0;
}
