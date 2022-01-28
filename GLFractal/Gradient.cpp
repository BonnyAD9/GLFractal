#include "Gradient.h"

Gradient::Gradient(ColorPoint* colorPoints, int count) : _colorPoints(colorPoints), _count(count) {}

ColorPoint* Gradient::colorPoints()
{
    return _colorPoints.get();
}

int Gradient::count()
{
    return _count;
}

unsigned char* Gradient::image(int width)
{
    if (width <= 0)
        return NULL;

    BVec3* arr = new BVec3[width];
    for (int i = 0; i < width; i++)
        arr[i] = getColor(i, width - 1);
    return (unsigned char*)arr;
}

BVec3 Gradient::getColor(float pos)
{
    ColorPoint* cp = _colorPoints.get();
    if (_count <= 0 || pos > 1.0f || pos < 0.0f)
        return BVec3{};

    int i;
    for (i = 1; i < _count && cp[i].position < pos; i++);

    ColorPoint p1 = cp[i - 1];
    ColorPoint p2 = cp[i];
    
    float mult = (pos - p1.position) / (p2.position - p1.position);
    return BVec3{
        (unsigned char)(p1.color.x + (float)(p2.color.x - p1.color.x) * mult),
        (unsigned char)(p1.color.y + (float)(p2.color.y - p1.color.y) * mult),
        (unsigned char)(p1.color.z + (float)(p2.color.z - p1.color.z) * mult),
    };
}

BVec3 Gradient::getColor(int pos, int max)
{
    if (pos < 0 || max <= 0)
        return BVec3{};
    return getColor((float)pos / max);
}

Gradient Gradient::fromPreset(GradientPreset preset)
{
    const unsigned int ultraFractalLen = 6;
    Gradient ultraFractal{ new ColorPoint[] {
        ColorPoint{ BVec3{ 0, 7, 100 }, 0.0f },
        ColorPoint{ BVec3{ 32, 107, 203 }, 0.16f },
        ColorPoint{ BVec3{ 237, 255, 255 }, 0.42f },
        ColorPoint{ BVec3{ 255, 170, 0 }, 0.6425f },
        ColorPoint{ BVec3{ 0, 2, 0 }, 0.8575f },
        ColorPoint{ BVec3{ 0, 7, 100 }, 1.0f },
    }, ultraFractalLen};

    const unsigned int monokaiLen = 6;
    Gradient monokai{ new ColorPoint[] {
        ColorPoint{ BVec3{ 39, 40, 34 }, 0.0f },
        ColorPoint{ BVec3{ 249, 38, 114 }, 0.2f },
        ColorPoint{ BVec3{ 102, 217, 239 }, 0.4f },
        ColorPoint{ BVec3{ 166, 226, 46 }, 0.6f },
        ColorPoint{ BVec3{ 253, 151, 31 }, 0.8f },
        ColorPoint{ BVec3{ 39, 40, 34 }, 1.0f },
    }, monokaiLen};

    const unsigned int grayscaleLen = 3;
    Gradient grayscale{ new ColorPoint[] {
        ColorPoint{ BVec3{ 0, 0, 0 }, 0.0f },
        ColorPoint{ BVec3{ 255, 255, 255 }, 0.5f },
        ColorPoint{ BVec3{ 0, 0, 0 }, 1.0f },
    }, grayscaleLen};

    switch (preset)
    {
    case GradientPreset::ULTRA_FRACTAL:
        return ultraFractal;
    case GradientPreset::MONOKAI:
        return monokai;
    case GradientPreset::GRAYSCALE:
        return grayscale;
    default:
        return grayscale;
    }
}