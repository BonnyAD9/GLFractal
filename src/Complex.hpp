#pragma once

#include "Vectors.hpp"

namespace Complex
{
    /// <summary>
    /// Multiplies two complex numbers
    /// </summary>
    /// <param name="a">First complex number</param>
    /// <param name="b">Second complex number</param>
    /// <returns>Product of the two complex numbers</returns>
    Vec2 cMul(Vec2 a, Vec2 b);
    /// <summary>
    /// Multiplies two complex numbers
    /// </summary>
    /// <param name="a">First complex number</param>
    /// <param name="b">Second complex number</param>
    /// <returns>Product of the two complex numbers</returns>
    DVec2 cMul(DVec2 a, DVec2 b);
}

