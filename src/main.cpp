#define GLEW_STATIC
#include <GL/glew.h>

#include "Shader.hpp"

#include <SFML/Window.hpp>

#include <memory>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

constexpr double PiOver180 = 3.141592653589793238/180;

sf::Window window;
float gameTime;

GLfloat cubeVertices[] {
	-1.0, -1.0,  0.0,		1.0, 0.0, 0.0,
	-1.0,  1.0,  0.0,		0.0, 1.0, 0.0,
	 1.0,  1.0,  0.0,		0.0, 0.0, 1.0,
	 1.0, -1.0,  0.0,		1.0, 0.0, 1.0,
};

GLuint VBO;

GLuint shaderProgram;
std::unique_ptr<Shader> vertexShader;
std::unique_ptr<Shader> fragmentShader;

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
	vertexShader = std::unique_ptr<Shader>(new Shader(Shader::Vertex));
	fragmentShader = std::unique_ptr<Shader>(new Shader(Shader::Fragment));

	vertexShader->loadFromFile("../shaders/main.vertex");
	fragmentShader->loadFromFile("../shaders/main.fragment");

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, *vertexShader);
	glAttachShader(shaderProgram, *fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");

	glClearColor(0.1, 0.1, 0.1, 1.0);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
}

void update(float dt)
{
	gameTime += dt;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_QUADS, 0, 4);
}

int main()
{
	window.create(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Demo", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	glewExperimental = GL_TRUE;
	glewInit();

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
