#ifndef GEOMETRY_GEOMETRY_HPP
#define GEOMETRY_GEOMETRY_HPP

/**
 * Represents a rectangle defined by two 2D points
 *
 * Top left is a (min, min) point, bottom right is a (max, max) point
 * It is guaranted that left <= right and top <= bottom
 */

#include <algorithm>
#include <utility>

template <typename T>
class Rectangle {
public:
    explicit Rectangle(T _left, T _top, T _right, T _bottom):
            left   (std::min(_left, _right )),
            top    (std::min(_top,  _bottom)),
            right  (std::max(_left, _right )),
            bottom (std::max(_top,  _bottom)) { }
    explicit Rectangle(std::pair <T, T> left_top, std::pair <T, T> right_bottom):
            left   (std::min(left_top.first,     left_top.second    )),
            top    (std::min(left_top.first,     left_top.second    )),
            right  (std::max(right_bottom.first, right_bottom.second)),
            bottom (std::max(right_bottom.first, right_bottom.second)) { }
    T height() {
        return bottom - top;
    }
    T width() {
        return right - left;
    }
// TODO: see below
#error TODO: getters and setters

protected:
    T left;
    T top;
    T right;
    T bottom;
};

#endif /* end of include guard: GEOMETRY_GEOMETRY_HPP */
