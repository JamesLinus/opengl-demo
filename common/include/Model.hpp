#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>
#include <map>

struct Model
{
	std::vector<Mesh> meshes;
	std::map<unsigned, std::string> textures;
};
