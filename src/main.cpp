#define GLEW_STATIC
#include <GL/glew.h>

#include "Shader.hpp"

#include "Model.hpp"

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <iostream>
#include <map>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

constexpr double PiOver180 = 3.141592653589793238/180;

sf::Window window;
float gameTime;

GLuint shaderProgram;
std::unique_ptr<Shader> vertexShader;
std::unique_ptr<Shader> fragmentShader;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
GLuint uniView;
GLuint uniModel;
GLuint uniNormalMatrix;
GLuint uniLightDirection;
GLuint uniLightPosition;
GLuint uniHalfVector;

glm::vec3 LightDirection = glm::normalize(glm::vec3(1, 0.1, 1));

std::map<std::string, std::unique_ptr<Model>> models;

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

	vertexShader->loadFromFile("../shaders/pointlight.vertex");
	fragmentShader->loadFromFile("../shaders/pointlight.fragment");

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, *vertexShader);
	glAttachShader(shaderProgram, *fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint uniProj   = glGetUniformLocation(shaderProgram, "projection");
	      uniView   = glGetUniformLocation(shaderProgram, "view");
	      uniModel  = glGetUniformLocation(shaderProgram, "model");
	      uniNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	      uniLightDirection = glGetUniformLocation(shaderProgram, "LightDirection");
	      uniLightPosition = glGetUniformLocation(shaderProgram, "LightPosition");
	      uniHalfVector = glGetUniformLocation(shaderProgram, "HalfVector");
	GLint uniAmbient = glGetUniformLocation(shaderProgram, "Ambient");

	glClearColor(0.1, 0.1, 0.1, 1.0);

	glEnable(GL_DEPTH_TEST);

	projection = glm::perspective(80.0f, (float) WIDTH / HEIGHT, 0.1f, 50.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	/** Lighting **/
	glUniform3f(uniAmbient, 0.1, 0.1, 0.2);

	/** Models **/
	models["world"] = std::unique_ptr<Model>(new Model("../res/world.obj", shaderProgram));
	models["teapot"] = std::unique_ptr<Model>(new Model("../res/teapot.obj", shaderProgram));
}

void update(float dt)
{
	gameTime += dt;

	view = glm::mat4();
	view = glm::translate(view, -glm::vec3(0, 0.5, 4.7));
	view = glm::rotate(view, gameTime*-20, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::vec3 lightDir = glm::mat3(view) * LightDirection;
	glUniform3fv(uniLightDirection, 1, glm::value_ptr(lightDir));

	glm::vec3 lightPos = glm::vec3(view * glm::vec4(0, -1, 0, 1));
	glUniform3fv(uniLightPosition, 1, glm::value_ptr(lightPos));

	glm::vec3 cameraDir(0, 0, 1);
	glm::vec3 halfVector = glm::normalize(cameraDir + lightDir);
	glUniform3fv(uniHalfVector, 1, glm::value_ptr(halfVector));

	glm::mat4 &model = models["teapot"]->modelMatrix;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 1, 0));
	model = glm::rotate(model, gameTime*50, glm::vec3(-0.6, 1, 0));
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto &model : models) {
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model.second->modelMatrix)));
		glUniformMatrix3fv(uniNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		model.second->render();
	}
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
