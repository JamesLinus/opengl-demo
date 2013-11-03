#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <map>

namespace ResourceManager
{
	GLuint aquireTexture(std::string name);
	void setSearchPath(const std::string &search_path);

	static std::map<std::string, GLuint> textures;
	static std::string search_path;
}
