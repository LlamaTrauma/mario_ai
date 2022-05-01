#include "helper.h"

float sigmoidActivation(float x){
    return 1 / (1 + pow(2.71828, -x));
}

void feedForwardMult(std::vector<float>* layer, std::vector<std::vector<float>*>* weights, std::vector<float>*& out){
    int rows = layer->size();
    int columns = out->size();
    for(int i = 0; i < columns; i ++){
        (*out)[i] = 0;
        for(int j = 0; j < rows; j ++){
            (*out)[i] += (*layer)[j] * (*(*weights)[j])[i];
        }
    }
}