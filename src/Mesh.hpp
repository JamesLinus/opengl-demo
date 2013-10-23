#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/scene.h>

class Mesh
{
public:
	Mesh(const aiMesh *, GLuint shaderProgram);
	~Mesh();

	void render() const;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	size_t numFaces;
	size_t numVertices;
};
