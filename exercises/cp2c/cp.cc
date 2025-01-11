#include <cmath>
#include <bits/stdc++.h>
using namespace std;

typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));
double4_t d8zero {
    0, 0, 0, 0
};

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
    std::vector<double> new_data(ny * nx);

    for (int i = 0; i < ny; ++i) {
        double row_sum = 0;
        for (int j = 0; j < nx; ++j) {
            row_sum += data[j + i * nx];
        }
        double row_mean = static_cast<double>(row_sum / nx);
        for (int j = 0; j < nx; ++j) {
            new_data[j + i * nx] = data[j + i * nx] - row_mean;
        }
    }

    for (int i = 0; i < ny; ++i) {
        double row_sqr = 0;
        for (int j = 0; j < nx; ++j) {
            row_sqr += static_cast<double>(new_data[j + i * nx] * new_data[j + i * nx]);
        }
        for (int j = 0; j < nx; ++j) {
            new_data[j + i * nx] = static_cast<double>(new_data[j + i * nx] / static_cast<double>(sqrt(row_sqr)));
        }
    }

    int nxb = ((nx / 4) + 1);
    std::vector<double4_t> appended_data(ny * nxb, d8zero);

    for (int i = 0; i < ny; ++i) {
        for (int j = 0; j < nxb; ++j) {
            for (int k = 0; k < 4; ++k) {
                if ((j * 4) + k == nx) {
                    break;
                }
                appended_data[j + i * nxb][k] = new_data[k + j * 4 + i * nx];
            }
        }
    }

    for (int i = 0; i < ny; ++i) {
        for (int j = i; j < ny; ++j) {
            double4_t vv = d8zero;
            for (int k = 0; k < nxb; ++k) {
                vv += (appended_data[k + i * nxb] * appended_data[k + j * nxb]);
            }
            double v = 0;
            for (int kb = 0; kb < 4; ++kb) {
                v += vv[kb];
            }
            result[ny * i + j] = v;
        } 
    }
}
