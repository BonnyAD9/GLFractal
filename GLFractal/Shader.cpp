#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>

#include "Shader.h"
#include "Vectors.h"

using std::cout, std::endl, std::istreambuf_iterator;

bool createShader(const char* source, GLuint* shader, GLenum type);
bool createShaderFromFile(const char* path, GLuint* shader, GLenum type);
bool createShaderProgram(const GLuint vertex, const GLuint fragment, GLuint* program);
bool createShaderProgram(const char* vertexSource, const char* fragmentSource, GLuint* program);
bool createShaderProgramFromFile(const char* vertexPath, const char* fragmentPath, GLuint* program);

Shader::Shader() : _id(0), _isCreated(false) {}

Shader::Shader(const char* vertexPath, const char* fragmentPath) : updateFun([](Shader& shader) {})
{
	if (!createShaderProgramFromFile(vertexPath, fragmentPath, &_id))
	{
		_isCreated = false;
		return;
	}
	_isCreated = true;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, function<void(Shader& shader)> update) : updateFun(update)
{
	if (!createShaderProgramFromFile(vertexPath, fragmentPath, &_id))
	{
		_isCreated = false;
		return;
	}
	_isCreated = true;
}

GLuint Shader::getId()
{
	return _id;
}

bool Shader::isCreated()
{
	return _isCreated;
}

void Shader::use()
{
	glUseProgram(_id);
}

void Shader::update()
{
	updateFun(*this);
}

void Shader::free()
{
	glDeleteProgram(_id);
}

void Shader::setInt(const char* name, GLint x)
{
	glUniform1i(glGetUniformLocation(_id, name), x);
}

void Shader::setFloat(const char* name, GLfloat x)
{
	glUniform1f(glGetUniformLocation(_id, name), x);
}

void Shader::setDouble(const char* name, GLdouble x)
{
	glUniform1d(glGetUniformLocation(_id, name), x);
}

void Shader::setFloat2(const char* name, GLfloat x, GLfloat y)
{
	glUniform2f(glGetUniformLocation(_id, name), x, y);
}

void Shader::setFloat2(const char* name, Vec2 xy)
{
	glUniform2f(glGetUniformLocation(_id, name), xy.x, xy.y);
}

void Shader::setDouble2(const char* name, GLdouble x, GLdouble y)
{
	glUniform2d(glGetUniformLocation(_id, name), x, y);
}

void Shader::setDouble2(const char* name, DVec2 xy)
{
	glUniform2d(glGetUniformLocation(_id, name), xy.x, xy.y);
}

void Shader::setFloat3(const char* name, GLfloat x, GLfloat y, GLfloat z)
{
	glUniform3f(glGetUniformLocation(_id, name), x, y, z);
}

void Shader::setFloat3(const char* name, Vec3 xyz)
{
	glUniform3f(glGetUniformLocation(_id, name), xyz.x, xyz.y, xyz.z);
}

void Shader::setMatrix4(const char* name, Mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(_id, name), 1, GL_TRUE, matrix.data());
}

void Shader::setMatrix4(const char* name, const float* data)
{
	glUniformMatrix4fv(glGetUniformLocation(_id, name), 1, GL_TRUE, data);
}

bool createShaderFromFile(const char* path, GLuint* shader, GLenum type)
{
	ifstream file(path);

	if (!file.is_open())
	{
		cout << "Couldn't open '" << path << "' file" << endl;
		return false;
	}

	string str(istreambuf_iterator<char>{file}, {});
	return createShader(str.c_str(), shader, type);
}

bool createShader(const char* source, GLuint* shader, GLenum type)
{
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &source, NULL);
	glCompileShader(*shader);

	int success;
	char infoLog[512];
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, NULL, infoLog);
		cout << "Couldn't compile vertex shader" << endl << infoLog << endl;
		return false;
	}

	return true;
}

bool createShaderProgram(const GLuint vertex, const GLuint fragment, GLuint* program)
{
	*program = glCreateProgram();

	glAttachShader(*program, vertex);
	glAttachShader(*program, fragment);
	glLinkProgram(*program);

	int success;
	char infoLog[512];
	glGetProgramiv(*program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(*program, 512, NULL, infoLog);
		cout << "Couldn't link shaders" << endl << infoLog << endl;
		return false;
	}

	return true;
}

bool createShaderProgram(const char* vertexSource, const char* fragmentSource, GLuint* program)
{
	GLuint vertex;
	if (!createShader(vertexSource, &vertex, GL_VERTEX_SHADER))
	{
		return false;
	}

	GLuint fragment;
	if (!createShader(fragmentSource, &fragment, GL_FRAGMENT_SHADER))
	{
		glDeleteShader(vertex);
		return false;
	}

	bool res = createShaderProgram(vertex, fragment, program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return res;
}

bool createShaderProgramFromFile(const char* vertexPath, const char* fragmentPath, GLuint* program)
{
	GLuint vertex;
	if (!createShaderFromFile(vertexPath, &vertex, GL_VERTEX_SHADER))
	{
		return false;
	}

	GLuint fragment;
	if (!createShaderFromFile(fragmentPath, &fragment, GL_FRAGMENT_SHADER))
	{
		glDeleteShader(vertex);
		return false;
	}

	bool res = createShaderProgram(vertex, fragment, program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return res;
}