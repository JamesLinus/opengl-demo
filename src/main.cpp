#include <SFML/Window.hpp>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

constexpr double PiOver180 = 3.141592653589793238/180;

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

}

int main()
{
	window.create(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Demo", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	init();

	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			handleEvent(event);
		}

		update(clock.restart().asSeconds());

		render();
		window.display();
	}

	return 0;
}
