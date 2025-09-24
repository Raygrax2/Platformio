#include "Filter.h"

void Filter::setup(const std::vector<float>& a_coeff, const std::vector<float>& b_coeff)
{
    a = a_coeff;
    b = b_coeff;

    // Initialize history vectors with zeros
    X.assign(b.size(), 0.0f);
    Y.assign(a.size(), 0.0f);
}

void Filter::apply(float input, float &output)
{
    // Shift input history
    for (int i = X.size() - 1; i > 0; i--)
        X[i] = X[i - 1];
    X[0] = input;

    // Shift output history
    for (int i = Y.size() - 1; i > 0; i--)
        Y[i] = Y[i - 1];

    // Compute new output
    output = 0.0f;
    for (size_t i = 0; i < b.size(); i++)
        output += b[i] * X[i];
    for (size_t i = 1; i < a.size(); i++)
        output -= a[i] * Y[i];

    output /= a[0]; // normalize by a[0]
    Y[0] = output;  // store output in history
}
