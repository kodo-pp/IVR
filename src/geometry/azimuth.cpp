#include <geometry/geometry.hpp>
#include <cmath>

double getAzimuth(double, double y, double) {
    // Only y is needed to perform necessary calculations
    return y * M_PI / 180;
}
