#define GLEW_STATIC
#include <GL/glew.h>

#include <Config.hpp>

#include <ResourceManager.hpp>
#include <AssimpLoader.hpp>
#include <GLModel.hpp>
#include <Shader.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/Image.hpp>

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
GLuint cubemapProgram;

glm::mat4 projection;
glm::mat4 view;

GLModel *model_teapot;

GLuint cubeTexture;

GLuint skybox_VAO;
GLuint skybox_VBO;
float skybox_vertices[] {
	// Front (but using back.jpg)
	-5,  5, -5,
	 5,  5, -5,
	 5, -5, -5,
	-5, -5, -5,

	// Right
	 5,  5, -5,
	 5, -5, -5,
	 5, -5,  5,
	 5,  5,  5,

	// Left
	-5,  5,  5,
	-5, -5,  5,
	-5, -5, -5,
	-5,  5, -5,

	// Back (but using front.jpg)
	 5,  5,  5,
	 5, -5,  5,
	-5, -5,  5,
	-5,  5,  5,

	// Down
	-5, -5, -5,
	-5, -5,  5,
	 5, -5,  5,
	 5, -5, -5,

	// Up
	-5,  5,  5,
	-5,  5, -5,
	 5,  5, -5,
	 5,  5,  5,
};

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

void initCubemap()
{
	Shader vertexShader(Shader::Vertex);
	Shader fragmentShader(Shader::Fragment);
	vertexShader.loadFromFile(search_path +"shaders/cubemap.vertex");
	fragmentShader.loadFromFile(search_path +"shaders/cubemap.fragment");

	cubemapProgram = glCreateProgram();
	glAttachShader(cubemapProgram, vertexShader);
	glAttachShader(cubemapProgram, fragmentShader);
	glBindAttribLocation(cubemapProgram, ATTRIBLOCATION_POSITION, "position");
	glLinkProgram(cubemapProgram);
	glUseProgram(cubemapProgram);

	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(ATTRIBLOCATION_POSITION);
	glVertexAttribPointer(ATTRIBLOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenTextures(1, &cubeTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

	sf::Image image;
	image.loadFromFile(search_path +"textures/left.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	image.loadFromFile(search_path +"textures/right.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	image.loadFromFile(search_path +"textures/down.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	image.loadFromFile(search_path +"textures/up.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	image.loadFromFile(search_path +"textures/back.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	image.loadFromFile(search_path +"textures/front.jpg");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glUniform1i(glGetUniformLocation(cubemapProgram, "cubeTexture"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
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

	initCubemap();

	projection = glm::perspective(90.f, 800.f/600.f, 1.f, 20.f);

	model_teapot = new GLModel(AssimpLoader::loadModelFromFile(search_path +"models/teapot.obj"));
}

void update(float dt)
{
	gameTime += dt;

	view = glm::mat4();
	view = glm::translate(view, glm::vec3(0, 0, -4));
	view = glm::rotate(view, gameTime*20, glm::vec3(0.5, 1, 0));
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
	glUseProgram(cubemapProgram);
	glm::mat4 model;
	glm::mat4 rotview = glm::mat4(glm::mat3(view));
	glUniformMatrix4fv(glGetUniformLocation(cubemapProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(cubemapProgram, "view"), 1, GL_FALSE, glm::value_ptr(rotview));
	glUniformMatrix4fv(glGetUniformLocation(cubemapProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(skybox_VAO);
	glDrawArrays(GL_QUADS, 0, 24);
	}

	{
	glUseProgram(shaderProgram);
	glm::mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	model_teapot->render();
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

	window.create(sf::VideoMode(WIDTH, HEIGHT), "Cubemap -- OpenGL Demo", sf::Style::Close, contextSettings);
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
