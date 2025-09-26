#ifndef _FILTER_H
#define _FILTER_H

#define MAX_NUM_SIZE 4 // Modify according to filter size
#define MAX_DEN_SIZE 4 // Modify according to filter size

class Filter
{
public:
    Filter();
    ~Filter();
    void setup(float b[], float a[], unsigned char num_coeff_b, unsigned char num_coeff_a, float x_init = 0, float y_init = 0);
    float apply(float input_data);

private:
    float *_b, *_a;
    float _x[MAX_NUM_SIZE], _y[MAX_DEN_SIZE];
    unsigned char _num_coeff[2];
};

#endif // _FILTER_H