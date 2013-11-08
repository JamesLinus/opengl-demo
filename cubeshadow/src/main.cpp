#define GLEW_STATIC
#include <GL/glew.h>

#include <Config.hpp>
#include <Shader.hpp>
#include <ResourceManager.hpp>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libgen.h>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

std::string search_path;

sf::Window window;
float gameTime;

GLuint shaderProgram;

GLuint uniProj;
glm::mat4 projection;

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
	glEnable(GL_DEPTH_TEST);

	Shader vertexShader(Shader::Vertex);
	Shader fragmentShader(Shader::Fragment);
	vertexShader.loadFromFile(search_path +"shaders/main.vertex");
	fragmentShader.loadFromFile(search_path +"shaders/main.fragment");

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindAttribLocation(shaderProgram, ATTRIBLOCATION_POSITION, "position");
	glBindAttribLocation(shaderProgram, ATTRIBLOCATION_NORMAL, "normal");
	glBindAttribLocation(shaderProgram, ATTRIBLOCATION_TEXCOORD, "texcoord");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	uniProj = glGetUniformLocation(shaderProgram, "projection");
	projection = glm::perspective(85.f, (float) WIDTH/HEIGHT, 1.f, 30.f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));
}

void update(float dt)
{
	gameTime += dt;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
