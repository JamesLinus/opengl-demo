#include "Shader.hpp"

#include <fstream>
#include <iterator>
#include <iostream>

Shader::Shader(Type type)
	: type(type)
{
	gl_name = glCreateShader(type);
}

Shader::~Shader()
{
	glDeleteShader(gl_name);
}

bool Shader::loadFromFile(std::string filename)
{
	std::ifstream file(filename);
	if ( ! file) {
		std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
		return false;
	}

	file >> std::noskipws;

	std::istream_iterator<char> it(file);
	std::istream_iterator<char> end;

	std::string source(it, end);

	const char *c_source = source.c_str();
	glShaderSource(gl_name, 1, &c_source, NULL);
	glCompileShader(gl_name);

	GLint status;
	glGetShaderiv(gl_name, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint len;
		glGetShaderiv(gl_name, GL_INFO_LOG_LENGTH, &len);
		GLchar *buffer = new GLchar[len];
		glGetShaderInfoLog(gl_name, len, NULL, buffer);

		std::cerr << "ERROR compiling \"" << filename << "\": " << buffer << std::endl;
		delete[] buffer;
		return false;
	}

	return true;
}

Shader::operator GLuint()
{
	return gl_name;
}
