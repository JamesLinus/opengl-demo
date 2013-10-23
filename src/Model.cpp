#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Model::Model(const std::string &filename, GLuint shaderProgram)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	if ( !scene) {
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "Model has " << scene->mNumMeshes << " meshes!" << std::endl;

	for (size_t i = 0; i < scene->mNumMeshes; i++)
		meshes.push_back(std::unique_ptr<Mesh>(new Mesh(scene->mMeshes[i], shaderProgram)));
}

Model::~Model()
{

}

void Model::render() const
{
	for (auto &mesh : meshes) {
		mesh->render();
	}
}
