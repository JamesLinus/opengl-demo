#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/scene.h>

class Mesh
{
public:
	Mesh(const aiMesh *mesh, const GLuint *textures, GLuint shaderProgram);
	~Mesh();

	void render() const;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint texture;

	size_t numFaces;
	size_t numVertices;
};
