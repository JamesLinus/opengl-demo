#include <AssimpLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

namespace AssimpLoader
{

Model loadModelFromFile(std::string filename)
{
	Model model;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	if ( ! scene) {
		std::cerr << importer.GetErrorString() << std::endl;
		return model;
	}

	std::cout << "Model has " << scene->mNumMaterials << " materials!" << std::endl;
	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial *material = scene->mMaterials[i];
		if ( ! material->GetTextureCount(aiTextureType_DIFFUSE)) {
			continue;
		}

		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::cout << "Texture: " << path.C_Str() << std::endl;
		model.textures[i] = path.C_Str();
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
		model.meshes.push_back(loadMesh(scene->mMeshes[i]));

	return model;
}

Mesh loadMesh(const aiMesh *sceneMesh)
{
	Mesh mesh;

	std::cout << "Mesh has " << sceneMesh->mNumVertices << " vertices and " << sceneMesh->mNumFaces << " faces, "
		<< "and it's using material #" << sceneMesh->mMaterialIndex << std::endl;

	size_t numVertices = sceneMesh->mNumVertices;
	size_t numFaces    = sceneMesh->mNumFaces;

	mesh.material_index = sceneMesh->mMaterialIndex;

	/** VERTICES **/
	mesh.vertices.reserve(numVertices);
	for (size_t i = 0; i < numVertices; i++)
		mesh.vertices.push_back(glm::vec3(
			sceneMesh->mVertices[i].x,
			sceneMesh->mVertices[i].y,
			sceneMesh->mVertices[i].z
		));

	/** NORMALS **/
	if (sceneMesh->HasNormals()) {
		mesh.normals.reserve(numVertices);
		for (size_t i = 0; i < numVertices; i++)
			mesh.normals.push_back(glm::vec3(
				sceneMesh->mNormals[i].x,
				sceneMesh->mNormals[i].y,
				sceneMesh->mNormals[i].z
			));
	}

	/** INDICES **/
	if (sceneMesh->HasFaces()) {
		mesh.indices.reserve(numFaces * 3);
		for (size_t i = 0; i < numFaces; i++) {
			mesh.indices.push_back(sceneMesh->mFaces[i].mIndices[0]);
			mesh.indices.push_back(sceneMesh->mFaces[i].mIndices[1]);
			mesh.indices.push_back(sceneMesh->mFaces[i].mIndices[2]);
		}
	}

	/** TEXTURE COORDINATES **/
	if (sceneMesh->HasTextureCoords(0)) {
		mesh.texcoords.reserve(numVertices);
		for (size_t i = 0; i < numVertices; i++)
			mesh.texcoords.push_back(glm::vec2(
				sceneMesh->mTextureCoords[0][i].x,
				sceneMesh->mTextureCoords[0][i].y
			));
	}

	return mesh;
}

}
