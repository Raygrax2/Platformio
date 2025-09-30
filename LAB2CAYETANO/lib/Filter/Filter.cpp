#include "Filter.h"

Filter::Filter()
{
    // initialize sizes to zero
    _num_coeff[0] = 0;
    _num_coeff[1] = 0;
    // zero arrays
    std::memset(_b, 0, sizeof(_b));
    std::memset(_a, 0, sizeof(_a));
    std::memset(_x, 0, sizeof(_x));
    std::memset(_y, 0, sizeof(_y));
}

Filter::~Filter() {}

void Filter::setup(const float b_arr[], const float a_arr[], unsigned char num_coeff_b, unsigned char num_coeff_a,
                   float x_init, float y_init)
{
    // bounds check (truncates if larger than max)
    unsigned char nb = std::min<unsigned char>(num_coeff_b, MAX_NUM_SIZE);
    unsigned char na = std::min<unsigned char>(num_coeff_a, MAX_DEN_SIZE);

    _num_coeff[0] = nb;
    _num_coeff[1] = na;

    // copy coefficients (safe copy)
    if (b_arr != nullptr)
        std::memcpy(_b, b_arr, nb * sizeof(float));
    else
        std::memset(_b, 0, nb * sizeof(float));

    if (a_arr != nullptr)
        std::memcpy(_a, a_arr, na * sizeof(float));
    else
        std::memset(_a, 0, na * sizeof(float));

    // initialize states with provided initial values
    for (unsigned char i = 0; i < MAX_NUM_SIZE; ++i) _x[i] = x_init;
    for (unsigned char i = 0; i < MAX_DEN_SIZE; ++i) _y[i] = y_init;
}

float Filter::apply(float input_data)
{
    // if filter not configured, just return input
    if (_num_coeff[0] == 0 || _num_coeff[1] == 0) return input_data;

    // shift X and Y arrays (right shift)
    for (int i = _num_coeff[0] - 1; i > 0; --i)
    {
        _x[i] = _x[i - 1];
    }
    for (int i = _num_coeff[1] - 1; i > 0; --i)
    {
        _y[i] = _y[i - 1];
    }

    // store new input
    _x[0] = input_data;

    // compute new output y[0] = sum(b[i]*x[i]) - sum_{i=1..na-1}(a[i]*y[i]) ; then divide by a[0] if needed
    float out = 0.0f;

    for (int i = 0; i < _num_coeff[0]; ++i)
    {
        out += _b[i] * _x[i];
    }

    for (int i = 1; i < _num_coeff[1]; ++i)
    {
        out -= _a[i] * _y[i];
    }

    // normalize by a[0] if not 1 (avoid division by zero)
    if (_a[0] != 0.0f && _a[0] != 1.0f)
    {
        out /= _a[0];
    }

    _y[0] = out;
    return _y[0];
}
