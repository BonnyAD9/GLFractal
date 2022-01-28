#pragma once

#include "Vectors.h"

#include <memory>

using std::unique_ptr;

struct ColorPoint
{
    BVec3 color{};
    float position{};
};

enum class GradientPreset
{
    ULTRA_FRACTAL,
    MONOKAI,
    GRAYSCALE,
};

class Gradient
{
public:
    Gradient(ColorPoint* colorPoints, int count);
    ColorPoint* colorPoints();
    int count();
    unsigned char* image(int width);
    BVec3 getColor(float pos);
    BVec3 getColor(int pos, int max);
    static Gradient fromPreset(GradientPreset preset);
private:
    unique_ptr<ColorPoint> _colorPoints;
    int _count;
};

