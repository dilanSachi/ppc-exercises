#include <cmath>
#include <bits/stdc++.h>
using namespace std;

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
    int i;
    int j;

    std::vector<double> new_data(ny * nx);

    for (i = 0; i < ny; i++) {
        double row_sum = 0;
        for (j = 0; j < nx; j++) {
            row_sum += data[j + i * nx];
        }
        double row_mean = static_cast<double>(row_sum / nx);
        for (j = 0; j < nx; j++) {
            new_data[j + i * nx] = data[j + i * nx] - row_mean;
        }
    }

    for (i = 0; i < ny; i++) {
        double row_sqr = 0;
        for (j = 0; j < nx; j++) {
            row_sqr += static_cast<double>(new_data[j + i * nx] * new_data[j + i * nx]);
        }
        for (j = 0; j < nx; j++) {
            new_data[j + i * nx] = static_cast<double>(new_data[j + i * nx] / static_cast<double>(sqrt(row_sqr)));
        }
    }

    int nxb = ((nx / 4) + 1) * 4;
    std::vector<double> appended_data(ny * nxb, 0);

    for (i = 0; i < ny; i++) {
        for (j = 0; j < nx; j++) {
            appended_data[j + i * nxb] = new_data[j + i * nx];
        }
    }

    for (int i = 0; i < ny; ++i) {
        for (int j = i; j < ny; ++j) {
            double vv[4];
            for (int kb = 0; kb < 4; ++kb) {
                vv[kb] = 0;
            }
            // double val1 = 0;
            // double val2 = 0;
            // double val3 = 0;
            // double val4 = 0;
            for (int k = 0; k < nxb / 4; ++k) {
                for (int l = 0; l < 4; ++l) {
                    vv[l] += static_cast<double>(appended_data[k * 4 + l + i * nxb] * appended_data[k * 4 + l + j * nxb]);
                }

                // if (k % 4 == 0) {
                //     val1 += static_cast<double>(new_data[k + i * nx] * new_data[k + j * nx]);
                // } else if (k % 4 == 1) {
                //     val2 += static_cast<double>(new_data[k + i * nx] * new_data[k + j * nx]);
                // } else if (k % 4 == 2) {
                //     val3 += static_cast<double>(new_data[k + i * nx] * new_data[k + j * nx]);
                // } else {
                //     val4 += static_cast<double>(new_data[k + i * nx] * new_data[k + j * nx]);
                // }
                // val1 += static_cast<double>(new_data[k + i * nx] * new_data[k + j * nx]);
            }
            double v = 0;
            for (int kb = 0; kb < 4; ++kb) {
                v += vv[kb];
            }
            result[ny * i + j] = v;
            // result[ny * i + j] = val1 + val2 + val3 + val4;
            // result[ny * i + j] = val1;
        } 
    }
}
