#define GLEW_STATIC
#include <GL/glew.h>

#include <ResourceManager.hpp>

#include <SFML/Window.hpp>

#include <libgen.h>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

std::string search_path;

sf::Window window;
float gameTime;

void handleEvent(sf::Event &event)
{
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Escape:
			window.close();
			break;
		default: break;
		}
		break;
	default: break;
	}
}

void init()
{
}

void update(float dt)
{
	gameTime += dt;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{
	search_path = {dirname(argv[0])};
	search_path += "/";
	ResourceManager::setSearchPath(search_path);

	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.antialiasingLevel = 8;

	window.create(sf::VideoMode(WIDTH, HEIGHT), "Omni-directional Shadow Mapping -- OpenGL Demo", sf::Style::Close, contextSettings);
	window.setVerticalSyncEnabled(true);

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	sf::Clock clock;

	while (window.isOpen()) {
		update(clock.restart().asSeconds());

		render();
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {
			handleEvent(event);
		}
	}

	return 0;
}
