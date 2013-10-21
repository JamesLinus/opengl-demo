#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>

class Shader
{
public:
	enum Type {
		Vertex   = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
	};

	Shader(Type hax);
	~Shader();

	bool loadFromFile(std::string filename);

	operator GLuint();

private:
	Type type;

	GLuint gl_name;
};
