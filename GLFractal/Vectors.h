#pragma once
#include <string>

using std::string;

/// <summary>
/// Vector of size 2 with single floating point precision
/// </summary>
class Vec2;
/// <summary>
/// Vector of size 2 with double floating point precision
/// </summary>
class DVec2;
/// <summary>
/// Vector of size 2 with integer precision
/// </summary>
class IVec2;
/// <summary>
/// Vector of size 3 with single floating point precision
/// </summary>
class Vec3;
/// <summary>
/// Matrix of size 4x4 with single floating point precision
/// </summary>
class Mat4;

class Vec2
{
public:
	/// <summary>
	/// Initializes 0 vector
	/// </summary>
	Vec2();
	/// <summary>
	/// Initializes vector from its components
	/// </summary>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	Vec2(float x, float y);
	/// <summary>
	/// Initializes vector from double vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	explicit Vec2(DVec2 vec);
	/// <summary>
	/// Initializes vector from int vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	explicit Vec2(IVec2 vec);
	Vec2 operator+(Vec2 vec);
	Vec2 operator-(Vec2 vec);
	Vec2 operator=(Vec2 vec);
	Vec2 operator+=(Vec2 vec);
	Vec2 operator*(float num);
	/// <summary>
	/// Converts this vector to string
	/// </summary>
	/// <returns>string representation of this vector</returns>
	string toString();
	/// <summary>
	/// Calculates length of this vector
	/// </summary>
	/// <returns>length of this vector</returns>
	float length();
	/// <summary>
	/// component x
	/// </summary>
	float x;
	/// <summary>
	/// component y
	/// </summary>
	float y;
};

class DVec2
{
public:
	/// <summary>
	/// Initializes 0 vector
	/// </summary>
	DVec2();
	/// <summary>
	/// Initializes vector from its components
	/// </summary>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	DVec2(double x, double y);
	/// <summary>
	/// Initializes vector vrom single vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	DVec2(Vec2 vec);
	/// <summary>
	/// Initializes vector from int vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	DVec2(IVec2 vec);
	DVec2 operator+(DVec2 vec);
	DVec2 operator-(DVec2 vec);
	DVec2 operator=(DVec2 vec);
	DVec2 operator+=(DVec2 vec);
	DVec2 operator*(double num);
	/// <summary>
	/// Converts this vector to string
	/// </summary>
	/// <returns>string representation of this vector</returns>
	string toString();
	/// <summary>
	/// Calculates length of this vector
	/// </summary>
	/// <returns>length of this vector</returns>
	double length();
	/// <summary>
	/// component x
	/// </summary>
	double x;
	/// <summary>
	/// component y
	/// </summary>
	double y;
};

class IVec2
{
public:
	/// <summary>
	/// Initializes 0 vector
	/// </summary>
	IVec2();
	/// <summary>
	/// Initializes vector from its components
	/// </summary>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	IVec2(int x, int y);
	/// <summary>
	/// Initializes vector from float vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	explicit IVec2(Vec2 vec);
	/// <summary>
	/// Initializes vector from double vector
	/// </summary>
	/// <param name="vec">vector to copy value from</param>
	explicit IVec2(DVec2 vec);
	IVec2 operator+(IVec2 vec);
	IVec2 operator-(IVec2 vec);
	IVec2 operator=(IVec2 vec);
	IVec2 operator+=(IVec2 vec);
	IVec2 operator*(int num);
	/// <summary>
	/// Converts this vector to string
	/// </summary>
	/// <returns>string representation of this vector</returns>
	string toString();
	/// <summary>
	/// Calculates length of this vector
	/// </summary>
	/// <returns>length of this vector</returns>
	float length();
	/// <summary>
	/// component x
	/// </summary>
	int x;
	/// <summary>
	/// component y
	/// </summary>
	int y;
};

class Vec3
{
public:
	/// <summary>
	/// Initializes 0 vector
	/// </summary>
	Vec3();
	/// <summary>
	/// Initializes vector from ints components
	/// </summary>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	/// <param name="z">component z</param>
	Vec3(float x, float y, float z);
	/// <summary>
	/// Initializes vector from Vec2 and z component
	/// </summary>
	/// <param name="xy">vector from which the x and y components will be copied</param>
	/// <param name="z">component z</param>
	Vec3(Vec2 xy, float z);
	/// <summary>
	/// Initializes vector from component x and Vec2
	/// </summary>
	/// <param name="x">component x</param>
	/// <param name="yz">vector from which the y and z components will be copied</param>
	Vec3(float x, Vec2 yz);
	Vec3 operator+(Vec3 vec);
	Vec3 operator-(Vec3 vec);
	Vec3 operator=(Vec3 vec);
	Vec3 operator+=(Vec3 vec);
	Vec3 operator*(float num);
	/// <summary>
	/// Converts this vector to string
	/// </summary>
	/// <returns>string representation of this vector</returns>
	string toString();
	/// <summary>
	/// Calculates length of this vector
	/// </summary>
	/// <returns>length of this vector</returns>
	float length();
	/// <summary>
	/// component x
	/// </summary>
	float x;
	/// <summary>
	/// component y
	/// </summary>
	float y;
	/// <summary>
	/// component z
	/// </summary>
	float z;
};

class Mat4
{
public:
	/// <summary>
	/// Initializes matrix filled with zeros
	/// </summary>
	Mat4();
	/// <summary>
	/// Width of the matrix (4)
	/// </summary>
	static const int WIDTH = 4;
	/// <summary>
	/// Height of the matrix (4)
	/// </summary>
	static const int HEIGHT = 4;
	/// <summary>
	/// Total number of elements (16)
	/// </summary>
	static const int LENGTH = WIDTH * HEIGHT;
	/// <summary>
	/// Initializes new matrix by copying data from the given array with length of 16
	/// </summary>
	/// <param name="data">Matrix array of size 16</param>
	Mat4(float data[LENGTH]);
	/// <summary>
	/// Gets value of the matrix at the given coordinates
	/// </summary>
	/// <param name="x">X coordinate</param>
	/// <param name="y">Y coordinate</param>
	/// <returns>value of the matrix at the given coordinates</returns>
	float at(int x, int y) const;
	/// <summary>
	/// Gets pointer to the matrix data
	/// </summary>
	/// <returns>pointer to the matrix data</returns>
	float* data();
	/// <summary>
	/// Creates new matrix with orthographic projection
	/// </summary>
	/// <param name="left">leftmost coordinate</param>
	/// <param name="right">rightmost coordinate</param>
	/// <param name="bottom">bottom coordinate</param>
	/// <param name="top">top coordinate</param>
	/// <param name="near">nearest coordinate</param>
	/// <param name="far">furtherest coordinate</param>
	/// <returns>Matrix with the given orthographic projection</returns>
	static Mat4 orthographic(
		const float left,
		const float right,
		const float bottom,
		const float top,
		const float near,
		const float far
	);
	/// <summary>
	/// Creates new scaling matrix
	/// </summary>
	/// <param name="xScale">Scale of x coordinate</param>
	/// <param name="yScale">Scale of y coordinate</param>
	/// <param name="zScale">Scale of z coordinate</param>
	/// <returns>Scaling matrix</returns>
	static Mat4 scale(const float xScale, const float yScale, const float zScale);
private:
	float _data[LENGTH];
};