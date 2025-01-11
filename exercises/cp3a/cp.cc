#include <cmath>
#include <bits/stdc++.h>
#include <x86intrin.h>
using namespace std;

typedef double double4_t __attribute__ ((vector_size (4 * sizeof(double))));
double4_t d8zero {
    0, 0, 0, 0
};

static inline double4_t swap2(double4_t x) { return _mm256_permute2f128_pd(x, x, 0b00000001); }
static inline double4_t swap1(double4_t x) { return _mm256_permute_pd(x, 0b10110101); }

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {

    double4_t val = {1, 2, 3, 4};
    cout << val[0] << " " << val[1] << " " << val[2] << " " << val[3] << endl;
    double4_t swapped1 = swap2(val);
    cout << swapped1[0] << " " << swapped1[1] << " " << swapped1[2] << " " << swapped1[3] << endl;
    double4_t swapped2 = swap1(val);
    cout << swapped2[0] << " " << swapped2[1] << " " << swapped2[2] << " " << swapped2[3] << endl;
    double4_t swapped3 = swap1(swapped1);
    cout << swapped3[0] << " " << swapped3[1] << " " << swapped3[2] << " " << swapped3[3] << endl;

    std::vector<double> new_data(ny * nx);

    #pragma omp parallel for
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

    #pragma omp parallel for
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
    int nyb = ((ny / 4) + 1);
    std::vector<double4_t> appended_data((nyb * 4) * nxb, d8zero);

    // #pragma omp parallel for
    // for (int i = 0; i < ny; ++i) {
    //     for (int j = 0; j < nxb; ++j) {
    //         for (int k = 0; k < 4; ++k) {
    //             if ((j * 4) + k == nx) {
    //                 break;
    //             }
    //             appended_data[j + i * nxb][k] = new_data[k + j * 4 + i * nx];
    //         }
    //     }
    // }

    #pragma omp parallel for
    for (int i = 0; i < nyb; ++i) {
        for (int j = 0; j < nx; ++j) {
            for (int k = 0; k < 4; ++k) {
                if ((i * 4) + k == ny) {
                    break;
                }
                appended_data[i * nx + j][k] = new_data[i * nx * 4 + j + k * nx];
            }
        }
    }

    // 1  2  3  4  5
    // 2  3  4  5  6
    // 3  4  5  6  7
    // 4  5  6  7  8
    // 5  6  7  8  9
    // 6  7  8  9  10
    // a b c d e
    // f g h i j
    // k l m n o
    // p q r s t

    #pragma omp parallel for
    for (int i = 0; i < nyb; ++i) {
        for (int j = i; j < ny; ++j) {
            double4_t vv1 = d8zero;
            double4_t vv2 = d8zero;
            double4_t vv3 = d8zero;
            double4_t vv4 = d8zero;
            for (int k = 0; k < nxb; ++k) {
                __builtin_prefetch(&appended_data[k + j * nxb + 20]);
                __builtin_prefetch(&appended_data[i * 4 * nxb + k + 20]);
                
                // double4_t sum_vec1;
                // double4_t sum_vec1;

                // double4_t vec1;
                // double4_t vec2;
                // double4_t sum_vec = vec1 * vec2;


                vv1 += (appended_data[k + ((i * 4) + 0) * nxb] * appended_data[k + j * nxb]);
                vv2 += (appended_data[k + ((i * 4) + 1) * nxb] * appended_data[k + j * nxb]);
                vv3 += (appended_data[k + ((i * 4) + 2) * nxb] * appended_data[k + j * nxb]);
                vv4 += (appended_data[k + ((i * 4) + 3) * nxb] * appended_data[k + j * nxb]);
            }
            double v[4] = {0, 0, 0, 0};
            for (int kb = 0; kb < 4; ++kb) {
                v[0] += vv1[kb];
                v[1] += vv2[kb];
                v[2] += vv3[kb];
                v[3] += vv4[kb];
            }
            for (int kc = 0; kc < std::min(4, ny - i * 4); ++kc) {
                result[ny * ((i * 4) + kc) + j] = v[kc];
            }
        } 
    }
}
