#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>

#include "Vectors.h"

using std::ifstream, std::function;

class Shader
{
public:
	/// <summary>
	/// Creates new shader from given files (check 'isCreated()' to ensure that shader was properly created)
	/// </summary>
	/// <param name="vertexPath">Path to vertex shader</param>
	/// <param name="fragmentPath">Path to fragment shader</param>
	Shader(const char* vertexPath, const char* fragmentPath);
	/// <summary>
	/// Creates invalid shader
	/// </summary>
	Shader();
	/// <summary>
	/// Creates new shader from given files and with update function (check 'isCreated()' to ensure that shader was properly created)
	/// </summary>
	/// <param name="vertexPath">Path to vertex shader</param>
	/// <param name="fragmentPath">Path to fragment shader</param>
	/// <param name="update">Function that will update data in shader</param>
	Shader(const char* vertexPath, const char* fragmentPath, function<void(Shader& shader)> update);
	/// <summary>
	/// Gets the shaders id
	/// </summary>
	/// <returns>This shaders ID</returns>
	GLuint getId();
	/// <summary>
	/// Shows whether this shader was initialized with no errors
	/// </summary>
	/// <returns>True if yes, false if not</returns>
	bool isCreated();
	/// <summary>
	/// Uses this shader
	/// </summary>
	void use();
	/// <summary>
	/// Updates data in this shader
	/// </summary>
	void update();
	/// <summary>
	/// Deletes this shader
	/// </summary>
	void free();
	/// <summary>
	/// Sets uniform int
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">value of variable</param>
	void setInt(const char* name, GLint x);
	/// <summary>
	/// Sets uniform float
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">value of variable</param>
	void setFloat(const char* name, GLfloat x);
	/// <summary>
	/// Sets uniform double
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">value of variable</param>
	void setDouble(const char* name, GLdouble x);
	/// <summary>
	/// Sets uniform vec2
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	void setFloat2(const char* name, GLfloat x, GLfloat y);
	/// <summary>
	/// Sets uniform vec2
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="xy">value of variable</param>
	void setFloat2(const char* name, Vec2 xy);
	/// <summary>
	/// Sets univorm dvec2
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	void setDouble2(const char* name, GLdouble x, GLdouble y);
	/// <summary>
	/// Sets uniform dvec2
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="xy">value of variable</param>
	void setDouble2(const char* name, DVec2 xy);
	/// <summary>
	/// Sets uniform vec3
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="x">component x</param>
	/// <param name="y">component y</param>
	/// <param name="z">component z</param>
	void setFloat3(const char* name, GLfloat x, GLfloat y, GLfloat z);
	/// <summary>
	/// Sets uniform vec3
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="xyz">value of variable</param>
	void setFloat3(const char* name, Vec3 xyz);
	/// <summary>
	/// Sets uniform mat4
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="matrix">value of variable</param>
	void setMatrix4(const char* name, Mat4 matrix);
	/// <summary>
	/// Sets uniform mat43
	/// </summary>
	/// <param name="name">name of variable</param>
	/// <param name="data">value of variable</param>
	void setMatrix4(const char* name, const float* data);
	/// <summary>
	/// Function that updates the value of shader
	/// </summary>
	function<void(Shader &shader)> updateFun;
private:
	GLuint _id;
	bool _isCreated;
};

