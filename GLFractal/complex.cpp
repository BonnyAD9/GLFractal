#include "complex.h"

namespace Complex
{
    Vec2 cMul(Vec2 a, Vec2 b)
    {
        return Vec2(
            a.x * b.x - a.y * b.y,
            a.x * b.y + a.y * b.x
        );
    }
}