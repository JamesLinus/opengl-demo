#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

class Model
{
public:
	Model(const std::string &filename, GLuint shaderProgram);
	~Model();

	void render() const;

	glm::mat4 modelMatrix;
private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	GLuint *textures;

	GLint uniModel;
};
