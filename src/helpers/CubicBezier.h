#pragma once

namespace helpers {
/**
 * @brief The Point data structure holds the information of a point.
*/
typedef struct Point
{
    float x;
    float y;

    Point()
    {
        x = 0.0f;
        y = 0.0f;
    }

    Point(float x, float y) : x(x), y(y) {}
} Point;

/**
 * @brief calculates the point on a cubic bezier curve at x = t
 * 
 * @param t the x value to calculate the point for
 * @param p0 the first control point of the curve
 * @param p1 the second control point of the curve
 * @param p2 the third control point of the curve
 * @param p3 the fourth control point of the curve
*/
Point cubicBezier(float t, Point &p0, Point &p1, Point &p2, Point &p3);
} // namespace helpers