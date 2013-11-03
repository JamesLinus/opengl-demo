#pragma once

#include <Model.hpp>

#include <GLMesh.hpp>

#include <vector>
#include <memory>

class GLModel
{
public:
	GLModel(Model model);

	void render() const;

	glm::mat4 model_matrix;

private:
	std::vector<std::unique_ptr<GLMesh>> meshes;
};
