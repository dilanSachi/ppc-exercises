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

    int nyb = ((ny / 4) + 1);
    int nybnew = (nx / 16) + 1;
    std::vector<double4_t> appended_data((nybnew * 4) * nx, d8zero);

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

    // a  b  c  d  e
    // f  g  h  i  j 
    // k  l  m  n  o
    // p  q  r  s  t
    // u  v  w  x  y
    // z aa bb cc dd

    // filling to the new array in the form
    // a f k p, b g l q, c h m r, d i n s, e j o t, u z _ _, v aa _ _, w bb _ _, x cc _ _, y dd _ _
    // #pragma omp parallel for
    // for (int i = 0; i < nyb; ++i) {
    //     for (int j = 0; j < nx; ++j) {
    //         for (int k = 0; k < 4; ++k) {
    //             if ((i * 4) + k == ny) {
    //                 break;
    //             }
    //             appended_data[i * nx + j][k] = new_data[i * nx * 4 + j + k * nx];
    //             // cout << "filling from " << i * nx * 4 + j + k * nx << " to " << i * nx + j << " and in " << k << endl;
    //         }
    //     }
    // }

    // a  b  c  d  e
    // f  g  h  i  j 
    // k  l  m  n  o
    // p  q  r  s  t
    // u  v  w  x  y
    // z aa bb cc dd

    // filling to the new array in the form
    // a f k p, u z _ _, b g l q, v aa _ _, c h m r, w bb _ _, d i n s, x cc _ _, e j o t, y dd _ _
    // #pragma omp parallel for
    for (int i = 0; i < nybnew * 4; ++i) {
        for (int j = 0; j < nx; ++j) {
            for (int k = 0; k < 4; ++k) {
                if ((i * 4) + k == ny) {
                    break;
                }
                if ((i * 16) + k == ny) {
                    break;
                }
                int filling_from = i * nx * 4 + j + k * nx;
                if (filling_from >= ny * nx) {
                    break;
                }
                int filling_to = j * nybnew * 4 + i;
                // cout << "filling from " << filling_from << " to " << filling_to << " " << k << endl;
                appended_data[filling_to][k] = new_data[filling_from];
            }
        }
    }

    // 1  2  3  4  5
    // 2  3  4  5  6
    // 3  4  5  6  7
    // 4  5  6  7  8
    // 5  6  7  8  9
    // 6  7  8  9  10

    // #pragma omp parallel for
    for (int i = 0; i < nybnew; ++i) {
        for (int j = i; j < nybnew * 4; ++j) {
            std::vector<double4_t> temp_sums(16, d8zero);
            for (int k = 0; k < nx; ++k) {
                // __builtin_prefetch(&appended_data[k + j * nxb + 20]);
                // __builtin_prefetch(&appended_data[i * 4 * nxb + k + 20]);
                
                double4_t vec1 = appended_data[i * 4 + k * nybnew * 4];
                double4_t vec2 = appended_data[i * 4 + k * nybnew * 4 + 1];
                double4_t vec3 = appended_data[i * 4 + k * nybnew * 4 + 2];
                double4_t vec4 = appended_data[i * 4 + k * nybnew * 4 + 3];

                // cout << "vec1 " << vec1[0] << " " << vec1[1] << " " << vec1[2] << " " << vec1[3] << " "<< endl;
                // cout << "vec2 " << vec2[0] << " " << vec2[1] << " " << vec2[2] << " " << vec2[3] << " "<< endl;
                // cout << "vec3 " << vec3[0] << " " << vec3[1] << " " << vec3[2] << " " << vec3[3] << " "<< endl;
                // cout << "vec4 " << vec4[0] << " " << vec4[1] << " " << vec4[2] << " " << vec4[3] << " "<< endl;

                double4_t target_vec1 = appended_data[j + k * nybnew * 4];
                // cout << "index of temp " << j + k * nybnew * 4 << endl;
                // cout << "target vec1 " << target_vec1[0] << " " << target_vec1[1] << " " << target_vec1[2] << " " << target_vec1[3] << " "<< endl;
                double4_t target_vec2 = swap2(target_vec1);
                // cout << "target vec2 " << target_vec2[0] << " " << target_vec2[1] << " " << target_vec2[2] << " " << target_vec2[3] << " "<< endl;
                double4_t target_vec3 = swap1(target_vec1);
                // cout << "target vec3 " << target_vec3[0] << " " << target_vec3[1] << " " << target_vec3[2] << " " << target_vec3[3] << " "<< endl;
                double4_t target_vec4 = swap1(target_vec2);
                // cout << "target vec4 " << target_vec4[0] << " " << target_vec4[1] << " " << target_vec4[2] << " " << target_vec4[3] << " "<< endl;

                temp_sums[0] += vec1 * target_vec1;
                temp_sums[1] += vec1 * target_vec2;
                temp_sums[2] += vec1 * target_vec3;
                temp_sums[3] += vec1 * target_vec4;

                // cout << "temp sums " << temp_sums[0][0] << " " << temp_sums[0][1] << " "  << temp_sums[0][2] << " "  << temp_sums[0][3] << "\n" << endl;

                temp_sums[4] += vec2 * target_vec1;
                temp_sums[5] += vec2 * target_vec2;
                temp_sums[6] += vec2 * target_vec3;
                temp_sums[7] += vec2 * target_vec4;

                temp_sums[8] += vec3 * target_vec1;
                temp_sums[9] += vec3 * target_vec2;
                temp_sums[10] += vec3 * target_vec3;
                temp_sums[11] += vec3 * target_vec4;

                temp_sums[12] += vec4 * target_vec1;
                temp_sums[13] += vec4 * target_vec2;
                temp_sums[14] += vec4 * target_vec3;
                temp_sums[15] += vec4 * target_vec4;
            }


            for (int r = 0; r < std::min(16, 16); ++r) {
                int dest = ny * (16 * i + r) + (j * 4);
                cout << "val " << dest << endl;
                if (dest >= ny * ny) continue;
                cout << "after break val " << dest << endl;
                result[dest] = temp_sums[r][0];
                dest = dest + ny + 1;
                cout << "val " << dest << endl;
                if (dest >= ny * ny) continue;
                cout << "after break val " << dest << endl;
                result[dest] = temp_sums[r][1];
                dest = dest + ny + 1;
                cout << "val " << dest << endl;
                if (dest >= ny * ny) continue;
                cout << "after break val " << dest << endl;
                result[dest] = temp_sums[r][2];
                dest = dest + ny + 1;
                cout << "val " << dest << endl;
                if (dest >= ny * ny) continue;
                cout << "after break val " << dest << endl;
                result[dest] = temp_sums[r][3];
            }

            // int x = i * 4 + 0;
            // int y = j * 4 + 0;
            // int source = (x % 16) + y % 4;
            // int dest = ny * (4 * x + x % 4) + y;
            // // cout << " source " << source << " dest " << dest << endl;
            // result[dest] = temp_sums[source][x % 4];
            
            // x = i * 4 + 1;
            // y = j * 4 + 1;
            // source = (x % 16) + y % 4;
            // dest = ny * (4 * x + x % 4) + y;
            // if (dest >= nx * ny) break;
            // // cout << " source " << source << " dest " << dest << endl;
            // result[dest] = temp_sums[source][x % 4];

            // x = i * 4 + 2;
            // y = j * 4 + 2;
            // source = (x % 16) + y % 4;
            // dest = ny * 4 * x + x % 4 + y;
            // if (dest >= nx * ny) break;
            // cout << " source " << source << " dest " << dest << endl;
            // result[dest] = temp_sums[source][x % 4];

            // x = i * 4 + 3;
            // y = j * 4 + 3;
            // source = (x % 16) + y % 4;
            // dest = ny * 4 * x + x % 4 + y;
            // if (dest >= nx * ny) break;
            // cout << " source " << source << " dest " << dest << endl;
            // result[dest] = temp_sums[source][x % 4];



            // 1 2 3 4
            // 3 4 1 2
            // 2 1 4 3
            // 4 3 2 1
        } 
    }



    // #pragma omp parallel for
    // for (int i = 0; i < nybnew; ++i) {
    //     for (int j = i; j < nyb; ++j) {
    //         double4_t vv1 = d8zero;
    //         double4_t vv2 = d8zero;
    //         double4_t vv3 = d8zero;
    //         double4_t vv4 = d8zero;
    //         for (int k = 0; k < nxb; ++k) {
    //             // __builtin_prefetch(&appended_data[k + j * nxb + 20]);
    //             // __builtin_prefetch(&appended_data[i * 4 * nxb + k + 20]);

    //             vv1 += (appended_data[k + ((i * 4) + 0) * nxb] * appended_data[k + j * nxb]);
    //             vv2 += (appended_data[k + ((i * 4) + 1) * nxb] * appended_data[k + j * nxb]);
    //             vv3 += (appended_data[k + ((i * 4) + 2) * nxb] * appended_data[k + j * nxb]);
    //             vv4 += (appended_data[k + ((i * 4) + 3) * nxb] * appended_data[k + j * nxb]);
    //         }
    //         double v[4] = {0, 0, 0, 0};
    //         for (int kb = 0; kb < 4; ++kb) {
    //             v[0] += vv1[kb];
    //             v[1] += vv2[kb];
    //             v[2] += vv3[kb];
    //             v[3] += vv4[kb];
    //         }
    //         for (int kc = 0; kc < std::min(4, ny - i * 4); ++kc) {
    //             result[ny * ((i * 4) + kc) + j] = v[kc];
    //         }
    //     } 
    // }
}
