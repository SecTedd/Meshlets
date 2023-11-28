#pragma once

namespace helpers {

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

Point cubicBezier(float t, Point &p0, Point &p1, Point &p2,
                  Point &p3);

} // namespace helpers