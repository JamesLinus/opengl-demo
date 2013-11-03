#pragma once

#include "Model.hpp"

#include <assimp/scene.h>

#include <string>

namespace AssimpLoader
{
	Model loadModelFromFile(std::string filename);

	Mesh loadMesh(const aiMesh *mesh);
}

