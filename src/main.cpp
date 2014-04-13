#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

SDL_Window* window;

int WIDTH = 1280;
int HEIGHT = 800;

GLuint compileShader(std::string filename, GLenum type)
{
	std::ifstream t(filename);
	std::string source(
		(std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>()
	);

	const char* source_str = source.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source_str, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if ( ! success) {
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		std::string info_log(log_length, '\0');
		glGetShaderInfoLog(shader, log_length, NULL, &info_log[0]);
		std::cerr << "Error compilating shader: " << info_log << std::endl;
		return 0;
	}

	return shader;
}

int main()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Skeletal Animation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
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

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.2, 0.2, 0.2, 1.0);

	bool is_running = true;
	while (is_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				is_running = false;
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		SDL_GL_SwapWindow(window);
	}
}
