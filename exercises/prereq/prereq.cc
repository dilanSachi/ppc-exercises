struct Result
{
    float avg[3];
};

/*
This is the function you need to implement. Quick reference:
- x coordinates: 0 <= x < nx
- y coordinates: 0 <= y < ny
- horizontal position: 0 <= x0 < x1 <= nx
- vertical position: 0 <= y0 < y1 <= ny
- color components: 0 <= c < 3
- input: data[c + 3 * x + 3 * nx * y]
- output: avg[c]
*/
Result calculate(int ny, int nx, const float *data, int y0, int x0, int y1, int x1)
{
    int x;
    int y;
    double c1 = 0;
    double c2 = 0;
    double c3 = 0;
    for (y = y0; y < y1; y++)
    {
        for (x = x0; x < x1; x++)
        {
            c1 = c1 + static_cast<double>(data[0 + 3 * x + 3 * nx * y]);
            c2 = c2 + static_cast<double>(data[1 + 3 * x + 3 * nx * y]);
            c3 = c3 + static_cast<double>(data[2 + 3 * x + 3 * nx * y]);
        }
    }
    int numberOfPixels = (x1 - x0) * (y1 - y0);
    Result result{{static_cast<float>(c1)/numberOfPixels, static_cast<float>(c2)/numberOfPixels, static_cast<float>(c3)/numberOfPixels}};
    return result;
}
