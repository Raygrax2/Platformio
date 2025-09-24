#ifndef FILTER_H
#define FILTER_H

#include <vector>

class Filter
{
public:
    Filter();

    // Setup with vectors of coefficients, this part was made with chatGPT
    void setup(const std::vector<float>& a_coeff, const std::vector<float>& b_coeff);
    float apply(float input, float &output);

private:
    std::vector<float> a; 
    std::vector<float> b; 
    std::vector<float> X; // input history
    std::vector<float> Y; // output history
};

#endif
