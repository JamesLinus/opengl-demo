#define GLEW_STATIC
#include <GL/glew.h>

#include <GLModel.hpp>
#include <Config.hpp>
#include <Shader.hpp>
#include <ResourceManager.hpp>
#include <AssimpLoader.hpp>

#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libgen.h>

#include <memory>
#include <map>

constexpr int WIDTH  = 800,
              HEIGHT = 600;

constexpr int shadowTextureSize = 512;

std::string search_path;

sf::Window window;
float gameTime;

GLuint shaderProgram;
GLuint programShadow;

GLuint uniProj;
GLuint uniView;
glm::mat4 projection;
glm::mat4 view;

glm::mat4 shadowProjection;
glm::mat4 shadowViews[6];

GLuint FBO;
GLuint shadowCubeTexture;

std::map<std::string, std::unique_ptr<GLModel>> models;

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

	{
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
	}

	{
	Shader vertexShader(Shader::Vertex);
	Shader fragmentShader(Shader::Fragment);
	vertexShader.loadFromFile(search_path +"shaders/shadow.vertex");
	fragmentShader.loadFromFile(search_path +"shaders/shadow.fragment");

	programShadow = glCreateProgram();
	glAttachShader(programShadow, vertexShader);
	glAttachShader(programShadow, fragmentShader);
	glBindAttribLocation(programShadow, ATTRIBLOCATION_POSITION, "position");
	glLinkProgram(programShadow);
	}

	uniProj = glGetUniformLocation(shaderProgram, "projection");
	uniView = glGetUniformLocation(shaderProgram, "view");
	projection = glm::perspective(85.f, (float) WIDTH/HEIGHT, 1.f, 30.f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	models["world"] = std::unique_ptr<GLModel>(new GLModel(AssimpLoader::loadModelFromFile(search_path +"../common/models/world.obj")));

	// Set up framebuffers
	glGenTextures(1, &shadowCubeTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeTexture);
	for (size_t i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, shadowTextureSize, shadowTextureSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	float color[] = { 0, 0, 0, 0 };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, color);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowProjection = glm::frustum(-1, 1, -1, 1, 1, 50);
	shadowViews[0] = glm::rotate(shadowViews[0], 90.f, glm::vec3(0, 1, 0));
	shadowViews[1] = glm::rotate(shadowViews[1], -90.f, glm::vec3(0, 1, 0));
	shadowViews[2] = glm::rotate(shadowViews[2], -90.f, glm::vec3(1, 0, 0));
	shadowViews[3] = glm::rotate(shadowViews[3], 90.f, glm::vec3(1, 0, 0));
	shadowViews[4] = glm::rotate(shadowViews[4], 180.f, glm::vec3(0, 1, 0));
}

void update(float dt)
{
	gameTime += dt;

	view = glm::mat4();
	view = glm::translate(view, -glm::vec3(0, 0, 5));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void render()
{
	// Draw shadow textures
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glUseProgram(programShadow);
	glViewport(0, 0, shadowTextureSize, shadowTextureSize);
	glm::mat4 lightview;
	glm::vec3 lightPosition;

	lightview = glm::translate(lightview, -lightPosition);
	glUniformMatrix4fv(glGetUniformLocation(programShadow, "lightView"), 1, GL_FALSE, glm::value_ptr(lightview));
	for (size_t i = 0; i < 6; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, shadowCubeTexture, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 theview = shadowViews[i] * lightview;
		glUniformMatrix4fv(glGetUniformLocation(programShadow, "view"), 1, GL_FALSE, glm::value_ptr(theview));
		glUniformMatrix4fv(glGetUniformLocation(programShadow, "projection"), 1, GL_FALSE, glm::value_ptr(shadowProjection));

		for (auto &model : models) {
			model.second->render();
		}
	}

	// Draw normal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shaderProgram);
	glViewport(0, 0, window.getSize().x, window.getSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowTexture"), 1);
	glActiveTexture(GL_TEXTURE0);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightView"), 1, GL_FALSE, glm::value_ptr(lightview));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	for (auto &model : models) {
		model.second->render();
	}
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
