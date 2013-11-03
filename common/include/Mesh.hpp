#pragma once

#include <glm/glm.hpp>

#include <vector>

struct Mesh
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<unsigned> indices;

	size_t material_index;
};
