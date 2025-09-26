#include "Filter.h"

// Constructor / destructor (empty - kept for clarity)
Filter::Filter() {}
Filter::~Filter() {}

void Filter::setup(float b_arr[], float a_arr[], unsigned char num_coeff_b, unsigned char num_coeff_a,
                   float x_init, float y_init)
{
    // store sizes
    int nb = num_coeff_b;
    int na = num_coeff_a;
    float X[num_coeff_b] = {0};
    float Y[num_coeff_a] = {0};
}

float Filter::apply(float input_data)
{
    
    // shift X and Y arrays
    for (int i = _num_coeff[0] - 1; i > 0; i--)
    {
        _x[i] = _x[i - 1];
    }
    for (int i = _num_coeff[1] - 1; i > 0; i--)
    {
        _y[i] = _y[i - 1];
    }
    // add new input
    _x[0] = input_data;
    // compute new output
    _y[0] = 0;
    for (int i = 0; i < _num_coeff[0]; i++)
    {
        _y[0] += _b[i] * _x[i];
    }

    for (int i = 1; i < _num_coeff[1]; i++)
    {
        _y[0] -= _a[i] * _y[i];
    }
    return _y[0];
}
