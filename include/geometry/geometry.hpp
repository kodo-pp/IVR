#ifndef GEOMETRY_GEOMETRY_HPP
#define GEOMETRY_GEOMETRY_HPP

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <modbox/geometry/game_position.hpp>

#include <irrlicht_wrapper.hpp>

/**
 * Represents a rectangle defined by two 2D points
 *
 * Top left is a (min, min) point, bottom right is a (max, max) point
 * It is guaranted that left <= right and top <= bottom
 */

template <typename T>
class Rectangle
{
public:
    explicit Rectangle(T _left, T _top, T _right, T _bottom)
            : left(std::min(_left, _right))
            , top(std::min(_top, _bottom))
            , right(std::max(_left, _right))
            , bottom(std::max(_top, _bottom))
    {
    }
    explicit Rectangle(std::pair<T, T> left_top, std::pair<T, T> right_bottom)
            : left(std::min(left_top.first, left_top.second))
            , top(std::min(left_top.first, left_top.second))
            , right(std::max(right_bottom.first, right_bottom.second))
            , bottom(std::max(right_bottom.first, right_bottom.second))
    {
    }
    T height()
    {
        return bottom - top;
    }
    T width()
    {
        return right - left;
    }

    T getLeft()
    {
        return left;
    }
    void setLeft(T newLeft)
    {
        if (newLeft > right) {
            throw std::range_error(
                    "The new 'left' value should not be greater than the current 'right' value");
        }
        left = newLeft;
    }

    T getRight()
    {
        return right;
    }
    void setRight(T newRight)
    {
        if (newRight < left) {
            throw std::range_error(
                    "The new 'right' value should not be less than the current 'left' value");
        }
        right = newRight;
    }

    T getTop()
    {
        return top;
    }
    void setTop(T newTop)
    {
        if (newTop > right) {
            throw std::range_error(
                    "The new 'top' value should not be greater than the current 'bottom' value");
        }
        top = newTop;
    }

    T getBottom()
    {
        return bottom;
    }
    void setBottom(T newBottom)
    {
        if (newBottom < top) {
            throw std::range_error(
                    "The new 'bottom' value should not be less than the current 'top' value");
        }
        bottom = newBottom;
    }

    void setLeftRight(T newLeft, T newRight)
    {
        if (newLeft > newRight) {
            throw std::range_error(
                    "The new 'left' value should not be greater than the new 'right' value");
        }
        left = newLeft;
        right = newRight;
    }

    void setTopBottom(T newTop, T newBottom)
    {
        if (newTop > newBottom) {
            throw std::range_error(
                    "The new 'top' value should not be greater than the new 'bottom' value");
        }
        top = newTop;
        bottom = newBottom;
    }

    Rectangle& operator=(const Rectangle& other)
    {
        left = other.left;
        right = other.right;
        top = other.top;
        bottom = other.bottom;
    }

    Rectangle& operator=(const Rectangle&& other)
    {
        left = other.left;
        right = other.right;
        top = other.top;
        bottom = other.bottom;
    }

protected:
    T left;
    T top;
    T right;
    T bottom;
};

/// Get (horizontal only) azimuth by rotation tuple (x, y, z)
double getAzimuth(double x, double y, double z);

irr::core::vector3df getRotationByTarget(const GamePosition& pos, const GamePosition& target);

#endif /* end of include guard: GEOMETRY_GEOMETRY_HPP */
