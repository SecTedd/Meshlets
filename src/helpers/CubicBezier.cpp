#include "CubicBezier.h"

namespace helpers {

Point cubicBezier(float t, Point &p0, Point &p1, Point &p2,
                  Point &p3)
{
    Point result;

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    // formula
    result.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
    result.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;

    return result;
}
} // namespace helpers