#include "Model.hpp"

#include "ResourceManager.hpp"

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

	std::cout << "Model has " << scene->mNumMaterials << " materials!" << std::endl;
	textures = new GLuint[scene->mNumMaterials];
	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial *material = scene->mMaterials[i];
		if ( ! material->GetTextureCount(aiTextureType_DIFFUSE)) {
			textures[i] = -1;
			continue;
		}

		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::cout << "Texture: " << path.C_Str() << std::endl;
		textures[i] = ResourceManager::aquireTexture(path.C_Str());
	}

	std::cout << "Model has " << scene->mNumMeshes << " meshes!" << std::endl;

	for (size_t i = 0; i < scene->mNumMeshes; i++)
		meshes.push_back(std::unique_ptr<Mesh>(new Mesh(scene->mMeshes[i], textures, shaderProgram)));

	uniModel = glGetUniformLocation(shaderProgram, "model");
}

Model::~Model()
{

}

void Model::render() const
{
	// glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	for (auto &mesh : meshes) {
		mesh->render();
	}
}
