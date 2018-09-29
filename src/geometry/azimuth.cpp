#include <cmath>

#include <modbox/geometry/geometry.hpp>

double getAzimuth(double /*unused*/, double y, double /*unused*/)
{
    // Only y is needed to perform necessary calculations
    return y * M_PI / 180;
}
