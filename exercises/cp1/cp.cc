#include <cmath>

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result)
{
    int i;
    int j;
    int k;
    for (i = 0; i < ny; i++)
    {
        for (j = 0; j <= i; j++)
        {
            double zig_xy = 0;
            double zig_x = 0;
            double zig_y = 0;
            double zig_x_sq = 0;
            double zig_y_sq = 0;
            for (k = 0; k < nx; k++)
            {
                zig_x += static_cast<double>(data[k + i * nx]);
                zig_y += static_cast<double>(data[k + j * nx]);
                zig_xy += static_cast<double>(data[k + i * nx]) * static_cast<double>(data[k + j * nx]);
                zig_x_sq += static_cast<double>(data[k + i * nx]) * static_cast<double>(data[k + i * nx]);
                zig_y_sq += static_cast<double>(data[k + j * nx]) * static_cast<double>(data[k + j * nx]);
            }
            result[i + j * ny] = ((nx * zig_xy) - (zig_x * zig_y))/(sqrt(((nx * zig_x_sq) - (zig_x * zig_x)) * ((nx * zig_y_sq) - (zig_y * zig_y))));
        }
    }
}
