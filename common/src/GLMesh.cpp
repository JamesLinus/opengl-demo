#include <GLMesh.hpp>

#include <Config.hpp>

#include <ResourceManager.hpp>

#include <iostream>

GLMesh::GLMesh(Mesh mesh, std::map<unsigned, std::string> &textures)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	size_t sizeVertices  = mesh.vertices.size() * sizeof(decltype(mesh.vertices)::value_type);
	size_t sizeNormals   = mesh.normals.size() * sizeof(decltype(mesh.normals)::value_type);
	size_t sizeTexcoords = mesh.texcoords.size() * sizeof(decltype(mesh.texcoords)::value_type);

	numIndices = mesh.indices.size();

	texture = ResourceManager::aquireTexture(textures[mesh.material_index]);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		mesh.indices.size() * sizeof(decltype(mesh.indices)::value_type),
		mesh.indices.data(),
		GL_STATIC_DRAW
	);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeVertices + sizeNormals + sizeTexcoords,
		NULL,
		GL_STATIC_DRAW
	);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, mesh.vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeNormals, mesh.normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals, sizeTexcoords, mesh.texcoords.data());

	glEnableVertexAttribArray(ATTRIBLOCATION_POSITION);
	glVertexAttribPointer(ATTRIBLOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(ATTRIBLOCATION_NORMAL);
	glVertexAttribPointer(ATTRIBLOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeVertices));

	glEnableVertexAttribArray(ATTRIBLOCATION_TEXCOORD);
	glVertexAttribPointer(ATTRIBLOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, (void *) (sizeVertices + sizeNormals));
}

GLMesh::~GLMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void GLMesh::render() const
{
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
}
