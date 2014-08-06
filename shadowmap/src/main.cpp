#define GLEW_STATIC
#include <GL/glew.h>

#include <ResourceManager.hpp>

#include <AssimpLoader.hpp>
#include <GLModel.hpp>

#include <Shader.hpp>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <libgen.h>

#include <memory>
#include <iostream>
#include <map>

int WIDTH  = 800,
    HEIGHT = 600;

constexpr double PiOver180 = 3.141592653589793238/180;

constexpr int shadowMapSize = 1024;

std::string search_path;

SDL_Window* window;
bool is_running = true;
float gameTime;

GLuint shaderProgram;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

glm::mat4 lightview;

GLuint uniView;
GLuint uniModel;
GLuint uniProj;
GLuint uniNormalMatrix;
GLuint uniLightDirection;
GLuint uniLightPosition;
GLuint uniHalfVector;

GLuint FBO;
GLuint shadowTexture;
GLuint shadowProgram;

glm::vec3 LightDirection = glm::normalize(glm::vec3(1, 0.1, 1));

std::map<std::string, std::unique_ptr<GLModel>> models;

void setPerspective()
{
	projection = glm::perspective(85.0f, (float) WIDTH / HEIGHT, 1.5f, 25.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));
	glViewport(0, 0, WIDTH, HEIGHT);
}

void handleEvent(SDL_Event &event)
{
	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			is_running = false;
			break;
		default: break;
		}
		break;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			WIDTH = event.window.data1;
			HEIGHT = event.window.data2;
			std::cout << "Resized to " << WIDTH << ", " << HEIGHT << std::endl;
			break;
		}
		break;
	default: break;
	}
}

void init()
{
	Shader vertexShader(Shader::Vertex);
	Shader fragmentShader(Shader::Fragment);

	vertexShader.loadFromFile(search_path +"shaders/pointlight.vertex");
	fragmentShader.loadFromFile(search_path +"shaders/pointlight.fragment");

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	Shader shadowVertexShader(Shader::Vertex);
	Shader shadowFragmentShader(Shader::Fragment);
	shadowVertexShader.loadFromFile(search_path +"shaders/shadowmap.vertex");
	shadowFragmentShader.loadFromFile(search_path +"shaders/shadowmap.fragment");
	shadowProgram = glCreateProgram();
	glAttachShader(shadowProgram, shadowVertexShader);
	glAttachShader(shadowProgram, shadowFragmentShader);
	glLinkProgram(shadowProgram);
	// glUseProgram(shadowProgram);

	      uniView   = glGetUniformLocation(shaderProgram, "view");
	      uniModel  = glGetUniformLocation(shaderProgram, "model");
	      uniProj   = glGetUniformLocation(shaderProgram, "projection");
	      uniNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	      uniLightDirection = glGetUniformLocation(shaderProgram, "LightDirection");
	      uniLightPosition = glGetUniformLocation(shaderProgram, "LightPosition");
	      uniHalfVector = glGetUniformLocation(shaderProgram, "HalfVector");
	GLint uniAmbient = glGetUniformLocation(shaderProgram, "Ambient");

	glEnable(GL_DEPTH_TEST);

	/** Lighting **/
	glUniform3f(uniAmbient, 0.1, 0.1, 0.2);

	/** Models **/
	models["world"] = std::unique_ptr<GLModel>(new GLModel(AssimpLoader::loadModelFromFile(search_path +"../common/models/world.obj")));
	models["teapot"] = std::unique_ptr<GLModel>(new GLModel(AssimpLoader::loadModelFromFile(search_path +"../common/models/teapot.obj")));

	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0, 0, 0, 0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update(float dt)
{
	gameTime += dt;

	glm::vec3 lightPosWorld = glm::vec3(4.5, 0, 0);
	lightPosWorld = glm::rotate(lightPosWorld, gameTime*-20, glm::vec3(0, 1, 0));

	view = glm::mat4();
	view = glm::translate(view, -glm::vec3(0, 0.5, 5.0));
	view = glm::rotate(view, gameTime*-5, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	lightview = glm::lookAt(lightPosWorld, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::vec3 lightDir = glm::mat3(view) * LightDirection;
	glUniform3fv(uniLightDirection, 1, glm::value_ptr(lightDir));

	glm::vec3 lightPos = glm::vec3(view * glm::vec4(lightPosWorld, 1.0));
	glUniform3fv(uniLightPosition, 1, glm::value_ptr(lightPos));

	glm::vec3 cameraDir(0, 0, 1);
	glm::vec3 halfVector = glm::normalize(cameraDir + lightDir);
	glUniform3fv(uniHalfVector, 1, glm::value_ptr(halfVector));

	glm::mat4 &model = models["teapot"]->model_matrix;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0, 1, 0));
	model = glm::rotate(model, gameTime*50, glm::vec3(-0.6, 1, 0));
}

void render()
{
	glUseProgram(shadowProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "lightview"), 1, GL_FALSE, glm::value_ptr(lightview));
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 lightproj = glm::frustum<float>(-1, 1, -1, 1, 1, 25);
	glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "projection"), 1, GL_FALSE, glm::value_ptr(lightproj));
	glViewport(0, 0, shadowMapSize, shadowMapSize);
	// glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

	for (auto &model : models) {
		glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "model"), 1, GL_FALSE, glm::value_ptr(model.second->model_matrix));
		model.second->render();
	}
	// return;

	// Draw again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shaderProgram);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setPerspective();

	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightview"), 1, GL_FALSE, glm::value_ptr(lightview));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightprojection"), 1, GL_FALSE, glm::value_ptr(lightproj));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowTexture"), 1);
	glActiveTexture(GL_TEXTURE0);

	for (auto &model : models) {
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model.second->model_matrix)));
		glUniformMatrix3fv(uniNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model.second->model_matrix));
		model.second->render();
	}
}

int main(int argc, char **argv)
{
	search_path = {dirname(argv[0])};
	search_path += "/";
	ResourceManager::setSearchPath(search_path);

	if (SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("OpenGL Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if ( ! window) {
		std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if ( ! context) {
		std::cerr << "Could not create GL context: " << SDL_GetError() << std::endl;
		exit(1);
	}

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Could not initialize GLEW: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	int major, minor;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	std::cout << "Got OpenGL " << major << "." << minor << std::endl;

	init();

	Uint32 ticks = 0;
	while (is_running) {
		Uint32 new_ticks = SDL_GetTicks();
		float dt = (new_ticks - ticks) / 1000.f;
		ticks = new_ticks;

		update(dt);

		render();
		SDL_GL_SwapWindow(window);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			handleEvent(event);
		}
	}

	return 0;
}
