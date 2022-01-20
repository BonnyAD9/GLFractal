#include "Vectors.h"

using std::to_string;

//==<<Vec2>>==//

Vec2::Vec2() : x(0.0f), y(0.0f) {}
Vec2::Vec2(float x, float y) : x(x), y(y) {}
Vec2::Vec2(DVec2 vec) : x((float)vec.x), y((float)vec.y) {}
Vec2::Vec2(IVec2 vec) : x((float)vec.x), y((float)vec.y) {}

Vec2 Vec2::operator+(Vec2 vec)
{
    return Vec2(x + vec.x, y + vec.y);
}

Vec2 Vec2::operator-(Vec2 vec)
{
    return Vec2(x - vec.x, y - vec.y);
}

Vec2 Vec2::operator=(Vec2 vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}

Vec2 Vec2::operator+=(Vec2 vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}

Vec2 Vec2::operator*(float num)
{
    return Vec2(x * num, y * num);
}

string Vec2::toString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ")";
}


//==<<DVec2>>==//

DVec2::DVec2() : x(0.0), y(0.0) {}
DVec2::DVec2(double x, double y) : x(x), y(y) {}
DVec2::DVec2(Vec2 vec) : x(vec.x), y(vec.y) {}
DVec2::DVec2(IVec2 vec) : x(vec.x), y(vec.y) {}

DVec2 DVec2::operator+(DVec2 vec)
{
    return DVec2(x + vec.x, y + vec.y);
}

DVec2 DVec2::operator-(DVec2 vec)
{
    return DVec2(x - vec.x, y - vec.y);
}

DVec2 DVec2::operator=(DVec2 vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}

DVec2 DVec2::operator+=(DVec2 vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}

DVec2 DVec2::operator*(double num)
{
    return DVec2(x * num, y * num);
}

string DVec2::toString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ")";
}

IVec2::IVec2() : x(0), y(0) {}
IVec2::IVec2(int x, int y) : x(x), y(y) {}
IVec2::IVec2(Vec2 vec) : x((int)vec.x), y((int)vec.y) {}
IVec2::IVec2(DVec2 vec) : x((int)vec.x), y((int)vec.y) {}

IVec2 IVec2::operator+(IVec2 vec)
{
    return IVec2(x + vec.x, y + vec.y);
}

IVec2 IVec2::operator-(IVec2 vec)
{
    return IVec2(x - vec.x, y - vec.y);
}

IVec2 IVec2::operator=(IVec2 vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}

IVec2 IVec2::operator+=(IVec2 vec)
{
    x += vec.x;
    y += vec.y;
    return *this;
}

IVec2 IVec2::operator*(int num)
{
    return IVec2(x * num, y * num);
}

string IVec2::toString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ")";
}


//==<<Vec3>>==//

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) {};
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
Vec3::Vec3(Vec2 xy, float z) : x(xy.x), y(xy.y), z(z) {}
Vec3::Vec3(float x, Vec2 yz) : x(x), y(yz.x), z(yz.y) {}

Vec3 Vec3::operator+(Vec3 vec)
{
    return Vec3(x + vec.x, y + vec.y, z + vec.z);
}

Vec3 Vec3::operator-(Vec3 vec)
{
    return Vec3(x - vec.x, y - vec.y, z - vec.z);
}

Vec3 Vec3::operator=(Vec3 vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

Vec3 Vec3::operator+=(Vec3 vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

Vec3 Vec3::operator*(float num)
{
    return Vec3(x * num, y * num, z * num);
}

string Vec3::toString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
}

//==<<Mat4>>==//

Mat4::Mat4() : _data{
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
} {}

Mat4::Mat4(float data[LENGTH])
{
    for (int i = 0; i < LENGTH; i++)
        _data[i] = data[i];
}

float Mat4::at(int x, int y) const
{
    return _data[x + WIDTH * y];
}

float* Mat4::data()
{
    return _data;
}

Mat4 Mat4::orthographic(
    const float left,
    const float right,
    const float bottom,
    const float top,
    const float near,
    const float far
)
{
    // credit: http://learnwebgl.brown37.net/08_projections/projections_ortho.html
    float arr[] = {
        2 / (right - left), 0                  , 0                , (left + right) / (left - right),
        0                 , 2 / (top - bottom) , 0                , (bottom + top) / (bottom - top),
        0                 , 0                  , -2 / (far - near), (near + far) / (near - far),
        0                 , 0                  , 0                , 1,
    };
    return Mat4(arr);
}

Mat4 Mat4::scale(const float xScale, const float yScale, const float zScale)
{
    float arr[] = {
        xScale, 0     , 0     , 0,
        0     , yScale, 0     , 0,
        0     , 0     , zScale, 0,
        0     , 0     , 0     , 1,
    };
    return Mat4(arr);
}