#pragma once
#include <string>

using std::string;

class Vec2;
class DVec2;
class IVec2;
class Vec3;

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
