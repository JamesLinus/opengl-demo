#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <Mesh.hpp>

#include <string>
#include <map>

class GLMesh
{
public:
	GLMesh(Mesh mesh, std::map<unsigned, std::string> &textures);
	~GLMesh();

	void render() const;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint texture;

	size_t numIndices;
};
