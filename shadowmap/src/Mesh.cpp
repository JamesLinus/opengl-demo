#include "Mesh.hpp"

#include <iostream>

Mesh::Mesh(const aiMesh *mesh, const GLuint *textures, GLuint shaderProgram)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	std::cout << "Mesh has " << mesh->mNumVertices << " vertices and " << mesh->mNumFaces << " faces, "
		<< "and it's using material #" << mesh->mMaterialIndex << std::endl;

	texture = textures[mesh->mMaterialIndex];
	numFaces = mesh->mNumFaces;
	numVertices = mesh->mNumVertices;

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	GLint normalAttrib = glGetAttribLocation(shaderProgram, "normal");
	GLint texAttrib = glGetAttribLocation(shaderProgram, "textureCoords");

	unsigned *indices = new unsigned[numFaces * 3];
	for (size_t i = 0; i < numFaces; i++)
		memcpy(indices + i*3, mesh->mFaces[i].mIndices, 3 * sizeof(unsigned));

	bool hasTextureCoords = mesh->HasTextureCoords(0);
	float *textureCoords;
	if (hasTextureCoords) {
		textureCoords = new float[numVertices*2];
		for (size_t i = 0; i < numVertices; i++) {
			textureCoords[i*2+0] = mesh->mTextureCoords[0][i].x;
			textureCoords[i*2+1] = mesh->mTextureCoords[0][i].y;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * 3 * sizeof(unsigned), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2*(numVertices*3*sizeof(float)) + numVertices*2*sizeof(float), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*3*sizeof(float), mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, numVertices*3*sizeof(float), numVertices*3*sizeof(float), mesh->mNormals);
	glBufferSubData(GL_ARRAY_BUFFER, 2*(numVertices*3*sizeof(float)), numVertices*2*sizeof(float), textureCoords);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void *) (numVertices*3*sizeof(float)));

	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void *) (2*(numVertices*3*sizeof(float))));
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::render() const
{
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, NULL);
}
