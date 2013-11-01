#include "ResourceManager.hpp"

#include <SFML/Graphics/Image.hpp>

namespace ResourceManager
{

GLuint aquireTexture(std::string name)
{
	if (textures.find(name) == textures.end()) {
		GLuint tex;
		glGenTextures(1, &tex);

		glBindTexture(GL_TEXTURE_2D, tex);

		sf::Image image;
		image.loadFromFile(search_path +"/res/"+ name);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		textures[name] = tex;
	}

	return textures[name];
}

void setSearchPath(const std::string &search_path)
{
	ResourceManager::search_path = search_path;
}

}
