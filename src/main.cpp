#define GLEW_STATIC
#include <GL/glew.h>

#include "Shader.hpp"

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <iostream>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

constexpr double PiOver180 = 3.141592653589793238/180;

sf::Window window;
float gameTime;

GLfloat cubeVertices[] {
	// Front
	-1.0, -1.0,  1.0,		1.0, 0.0, 0.0,		0.0, 0.0, 1.0,
	-1.0,  1.0,  1.0,		0.0, 1.0, 0.0,		0.0, 0.0, 1.0,
	 1.0,  1.0,  1.0,		0.0, 0.0, 1.0,		0.0, 0.0, 1.0,
	 1.0, -1.0,  1.0,		1.0, 0.0, 1.0,		0.0, 0.0, 1.0,

	// Back
	-1.0, -1.0, -1.0,		0.0, 0.0, 1.0,		0.0, 0.0, -1.0,
	-1.0,  1.0, -1.0,		1.0, 0.0, 1.0,		0.0, 0.0, -1.0,
	 1.0,  1.0, -1.0,		1.0, 0.0, 0.0,		0.0, 0.0, -1.0,
	 1.0, -1.0, -1.0,		0.0, 1.0, 0.0,		0.0, 0.0, -1.0,

	// Right
	 1.0, -1.0,  1.0,		1.0, 0.0, 1.0,		1.0, 0.0, 0.0,
	 1.0,  1.0,  1.0,		0.0, 0.0, 1.0,		1.0, 0.0, 0.0,
	 1.0,  1.0, -1.0,		1.0, 0.0, 0.0,		1.0, 0.0, 0.0,
	 1.0, -1.0, -1.0,		0.0, 1.0, 0.0,		1.0, 0.0, 0.0,

	// Left
	-1.0, -1.0,  1.0,		1.0, 0.0, 0.0,		-1.0, 0.0, 0.0,
	-1.0,  1.0,  1.0,		0.0, 1.0, 0.0,		-1.0, 0.0, 0.0,
	-1.0,  1.0, -1.0,		1.0, 0.0, 1.0,		-1.0, 0.0, 0.0,
	-1.0, -1.0, -1.0,		0.0, 0.0, 1.0,		-1.0, 0.0, 0.0,

	// Top
	 1.0,  1.0,  1.0,		0.0, 0.0, 1.0,		0.0, 1.0, 0.0,
	-1.0,  1.0,  1.0,		0.0, 1.0, 0.0,		0.0, 1.0, 0.0,
	-1.0,  1.0, -1.0,		1.0, 0.0, 1.0,		0.0, 1.0, 0.0,
	 1.0,  1.0, -1.0,		1.0, 0.0, 0.0,		0.0, 1.0, 0.0,

	// Bottom
	 1.0, -1.0,  1.0,		1.0, 0.0, 1.0,		0.0, -1.0, 0.0,
	-1.0, -1.0,  1.0,		1.0, 0.0, 0.0,		0.0, -1.0, 0.0,
	-1.0, -1.0, -1.0,		0.0, 0.0, 1.0,		0.0, -1.0, 0.0,
	 1.0, -1.0, -1.0,		0.0, 1.0, 0.0,		0.0, -1.0, 0.0,
};

GLfloat floorVertices[] {
	-5.0, -2.0, -5.0,		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
	-5.0, -2.0,  5.0,		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
	 5.0, -2.0,  5.0,		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
	 5.0, -2.0, -5.0,		0.5, 0.5, 0.5,		0.0, 1.0, 0.0,
};

GLuint cube_VAO;
GLuint cube_VBO;

GLuint floor_VAO;
GLuint floor_VBO;

GLuint shaderProgram;
std::unique_ptr<Shader> vertexShader;
std::unique_ptr<Shader> fragmentShader;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
GLuint uniView;
GLuint uniModel;
GLuint uniNormalMatrix;

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
	GLint normalAttrib = glGetAttribLocation(shaderProgram, "normal");
	GLint uniProj   = glGetUniformLocation(shaderProgram, "projection");
	      uniView   = glGetUniformLocation(shaderProgram, "view");
	      uniModel  = glGetUniformLocation(shaderProgram, "model");
	      uniNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	GLint uniAmbient = glGetUniformLocation(shaderProgram, "Ambient");

	glClearColor(0.1, 0.1, 0.1, 1.0);

	glEnable(GL_DEPTH_TEST);

	/** CUBE **/
	glGenVertexArrays(1, &cube_VAO);
	glGenBuffers(1, &cube_VBO);

	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));

	/** FLOOR **/
	glGenVertexArrays(1, &floor_VAO);
	glGenBuffers(1, &floor_VBO);

	glBindVertexArray(floor_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
		9 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));

	projection = glm::perspective(80.0f, (float) WIDTH / HEIGHT, 0.1f, 50.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	/** Lighting **/
	glUniform3f(uniAmbient, 0.2, 0.2, 0.4);
}

void update(float dt)
{
	gameTime += dt;

	view = glm::mat4();
	view = glm::translate(view, -glm::vec3(0, 0, 4));
	view = glm::rotate(view, gameTime*-10, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void pushNormalMatrix()
{
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	glUniformMatrix3fv(uniNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4();
	glBindVertexArray(floor_VAO);
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	pushNormalMatrix();
	glDrawArrays(GL_QUADS, 0, 4);

	model = glm::rotate(model, gameTime*50, glm::vec3(-0.6, 1, 0));
	glBindVertexArray(cube_VAO);
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	pushNormalMatrix();
	glDrawArrays(GL_QUADS, 0, 24);
}

int main()
{
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.antialiasingLevel = 8;

	window.create(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Demo", sf::Style::Close, contextSettings);
	window.setVerticalSyncEnabled(true);

	contextSettings = window.getSettings();
	std::cout
		<< "DEPTH BITS: " << contextSettings.depthBits << std::endl
		<< "ANTIALIAS: " << contextSettings.antialiasingLevel << std::endl
		;

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
