#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <map>

namespace ResourceManager
{
	GLuint aquireTexture(std::string name);

	static std::map<std::string, GLuint> textures;
}
