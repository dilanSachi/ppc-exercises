#include <cmath>
#include <bits/stdc++.h>
using namespace std;

typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));
float8_t f8zero {
    0, 0, 0, 0, 0, 0, 0, 0
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
    std::vector<float> new_data(ny * nx);

    #pragma omp parallel for
    for (int i = 0; i < ny; ++i) {
        float row_sum = 0;
        for (int j = 0; j < nx; ++j) {
            row_sum += data[j + i * nx];
        }
        float row_mean = static_cast<float>(row_sum / nx);
        for (int j = 0; j < nx; ++j) {
            new_data[j + i * nx] = data[j + i * nx] - row_mean;
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < ny; ++i) {
        float row_sqr = 0;
        for (int j = 0; j < nx; ++j) {
            row_sqr += static_cast<float>(new_data[j + i * nx] * new_data[j + i * nx]);
        }
        for (int j = 0; j < nx; ++j) {
            new_data[j + i * nx] = static_cast<float>(new_data[j + i * nx] / static_cast<float>(sqrt(row_sqr)));
        }
    }

    int nxb = ((nx / 8) + 1);
    int nyb = ((ny / 8) + 1);
    std::vector<float8_t> appended_data((nyb * 8) * nxb, f8zero);

    #pragma omp parallel for
    for (int i = 0; i < ny; ++i) {
        for (int j = 0; j < nxb; ++j) {
            for (int k = 0; k < 8; ++k) {
                if ((j * 8) + k == nx) {
                    break;
                }
                appended_data[j + i * nxb][k] = new_data[k + j * 8 + i * nx];
            }
        }
    }

    // 1  2  3  4  5
    // 2  3  4  5  6
    // 3  4  5  6  7
    // 4  5  6  7  8
    // 5  6  7  8  9
    // 6  7  8  9  10

    #pragma omp parallel for
    for (int i = 0; i < nyb; ++i) {
        // #pragma omp parallel for
        for (int j = i; j < ny; ++j) {
            float8_t vv1 = f8zero;
            float8_t vv2 = f8zero;
            float8_t vv3 = f8zero;
            float8_t vv4 = f8zero;
            float8_t vv5 = f8zero;
            float8_t vv6 = f8zero;
            float8_t vv7 = f8zero;
            float8_t vv8 = f8zero;
            for (int k = 0; k < nxb; ++k) {
                __builtin_prefetch(&appended_data[k + j * nxb + 20]);
                __builtin_prefetch(&appended_data[i * 8 * nxb + k + 20]);
                vv1 += (appended_data[k + ((i * 8) + 0) * nxb] * appended_data[k + j * nxb]);
                vv2 += (appended_data[k + ((i * 8) + 1) * nxb] * appended_data[k + j * nxb]);
                vv3 += (appended_data[k + ((i * 8) + 2) * nxb] * appended_data[k + j * nxb]);
                vv4 += (appended_data[k + ((i * 8) + 3) * nxb] * appended_data[k + j * nxb]);
                vv5 += (appended_data[k + ((i * 8) + 4) * nxb] * appended_data[k + j * nxb]);
                vv6 += (appended_data[k + ((i * 8) + 5) * nxb] * appended_data[k + j * nxb]);
                vv7 += (appended_data[k + ((i * 8) + 6) * nxb] * appended_data[k + j * nxb]);
                vv8 += (appended_data[k + ((i * 8) + 7) * nxb] * appended_data[k + j * nxb]);
            }
            float v[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            for (int kb = 0; kb < 8; ++kb) {
                v[0] += vv1[kb];
                v[1] += vv2[kb];
                v[2] += vv3[kb];
                v[3] += vv4[kb];
                v[4] += vv5[kb];
                v[5] += vv6[kb];
                v[6] += vv7[kb];
                v[7] += vv8[kb];
            }
            for (int kc = 0; kc < std::min(8, ny - i * 8); ++kc) {
                result[ny * ((i * 8) + kc) + j] = v[kc];
            }
        } 
    }
}
