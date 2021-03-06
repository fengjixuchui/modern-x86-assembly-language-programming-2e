//------------------------------------------------
//               Ch15_01.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include "AlignedMem.h"

using namespace std;

extern "C" int CalcResult_(double* y, const double* x, size_t n);

int main()
{
    const size_t n = 1024;
    AlignedArray<double> x_aa(n, 32);
    AlignedArray<double> y_aa(n, 32);

    double* x = x_aa.Data();
    double* y = y_aa.Data();

    for (size_t i = 0; i < n; i++)
        x[i] = (double)i;

    int rc_main = 1;
    int rc = CalcResult_(y, x, n);

    if (rc != 0)
        cout << "CalcResult_ failed - rc = " << rc << '\n';
    else
    {
        size_t num_diff = 0;

        for (size_t i = 0; i < n; i++)
        {
            if (y[i] != sqrt(x[i] / 2.0))
            {
                if (num_diff < 5)
                    cout << "compare error = index = " << i << '\n';

                num_diff++;
            }
        }

        rc_main = (num_diff == 0) ? 0 : 1;
        cout << "n = " << n << '\n';
        cout << "num_diff = " << num_diff << '\n';
    }

    return rc_main;
}
