#define GLM_FORCE_RADIANS

#include "utils.hpp"

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

SDL_Window* window;

int WIDTH = 800;
int HEIGHT = 600;

Assimp::Importer importer;

struct Model
{
	struct Node {
		std::string name;
		glm::mat4 transform;
		Node* parent;
		std::vector<std::unique_ptr<Node>> children;
	};

	struct Animation {
		struct Node {
			std::string name;
			std::vector<std::pair<float, glm::vec3>> positions;
			std::vector<std::pair<float, glm::quat>> rotations;
			std::vector<std::pair<float, glm::vec3>> scales;
		};

		std::string name;
		std::map<std::string, Node> nodes;
		double ticks_per_second;
		double duration;
	};

	struct Mesh {
		struct Vertex {
			glm::vec3 position;
			std::map<unsigned, float> weights;
		};

		struct BoneInfo {
			glm::mat4 final_transform;
			glm::mat4 offset;
		};

		Model* model;
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
		std::vector<BoneInfo> bone_info;
		std::map<std::string, size_t> bone_map;
		size_t num_bones;

		size_t find_rotation(float time, Model::Animation::Node& node)
		{
			for (size_t i = 0; i < node.rotations.size(); ++i) {
				if (time < node.rotations[i+1].first) {
					return i;
				}
			}

			assert(false);
		}

		void read_node_hierarchy(float animation_time, const Node* node, const glm::mat4 parent_transform)
		{
			std::cout << std::endl << "Reading node " << node->name << std::endl;

			glm::mat4 node_transformation = node->transform;
			std::cout << "ORIGINAL NODE TRANSFORM IS:" << std::endl;
			print_matrix(node_transformation);

			auto animation_nodes = model->animations[0].nodes;
			if (animation_nodes.find(node->name) != animation_nodes.end()) {
				auto animation_node = animation_nodes[node->name];

				// Position
				std::cout << "--- It has " << animation_node.positions.size() << " positions" << std::endl;
				glm::mat4 position_matrix;
				glm::vec3 position = animation_node.positions[1].second;
				std::cout << "Translating by " << position << std::endl;
				position_matrix = glm::translate(position_matrix, position);

				// Rotation
				std::cout << "--- It has " << animation_node.rotations.size() << " rotations" << std::endl;
				auto rotation_index = find_rotation(animation_time, animation_node);
				auto rotation_index_next = rotation_index + 1;
				float dt = animation_node.rotations[rotation_index_next].first
					- animation_node.rotations[rotation_index].first;
				float factor = (animation_time - animation_node.rotations[rotation_index].first) / dt;
				auto start_rotation = (animation_node.rotations[rotation_index].second);
				auto end_rotation = (animation_node.rotations[rotation_index_next].second);
				auto rotation = glm::mix(start_rotation, end_rotation, factor);
				std::cout << "Rotating by " << rotation << std::endl;
				glm::mat4 rotation_matrix = glm::mat4_cast(rotation);

				node_transformation = position_matrix * rotation_matrix;
			}

			std::cout << "NEW NODE TRANSFORM IS:" << std::endl;
			print_matrix(node_transformation);

			glm::mat4 global_transformation = parent_transform * node_transformation;

			if (bone_map.find(node->name) != bone_map.end()) {
				size_t bone_id = bone_map[node->name];
				std::cout << "===== Setting final transform for " << node->name << std::endl;
				bone_info[bone_id].final_transform
					= model->global_inverse_transform * global_transformation * bone_info[bone_id].offset;
				print_matrix(bone_info[bone_id].final_transform);
			}

			std::cout << " it has " << node->children.size() << " children" << std::endl;
			for (auto &child_node : node->children) {
				read_node_hierarchy(animation_time, child_node.get(), global_transformation);
			}
		}

		std::vector<glm::mat4> get_transforms(float time)
		{
			std::cout << "Getting transformations as time " << time << std::endl;
			std::cout << "Root node has " << model->root_node->children.size() << " children" << std::endl;

			auto animation = model->animations[0];
			float time_in_ticks = time * animation.ticks_per_second;
			float animation_time = fmod(time_in_ticks, animation.duration);

			glm::mat4 identity;
			read_node_hierarchy(animation_time, model->root_node.get(), identity);

			std::vector<glm::mat4> transforms;
			transforms.resize(num_bones);
			for (size_t i = 0; i < num_bones; ++i) {
				transforms[i] = bone_info[i].final_transform;
			}
			return transforms;
		}
	};

	std::unique_ptr<Node> root_node;
	std::vector<Mesh> meshes;
	std::vector<Animation> animations;
	glm::mat4 global_inverse_transform;
};

std::unique_ptr<Model::Node> process_node(aiNode* node, size_t levels = 0)
{
	auto model_node = std::unique_ptr<Model::Node>(new Model::Node);

	std::stringstream ss;
	for (size_t i = 0; i < levels; ++i) {
		ss << "  ";
	}
	std::string prefix(ss.str());

	std::cout << std::endl << prefix << "Node is named " << node->mName.C_Str() << std::endl;
	std::cout << prefix << "It has " << node->mNumChildren << " children" << std::endl;
	std::cout << prefix << "Its transformation is" << std::endl;
	print_matrix(assimp_to_glm(node->mTransformation));

	model_node->name = {node->mName.C_Str()};
	model_node->transform = assimp_to_glm(node->mTransformation);

	for (size_t i = 0; i < node->mNumChildren; ++i) {
		auto child_node = process_node(node->mChildren[i], levels+1);
		child_node->parent = model_node.get();
		model_node->children.push_back(std::move(child_node));
	}

	return model_node;
}

Model import_model(std::string filename)
{
	Model model;

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if ( ! scene) {
		std::cerr << "Could not import model: " << importer.GetErrorString() << std::endl;
		return model;
	}

	model.root_node = process_node(scene->mRootNode);
	model.global_inverse_transform = glm::inverse(model.root_node->transform);

	std::cout << std::endl << "Model has " << scene->mNumAnimations << " animations" << std::endl;
	for (size_t i = 0; i < scene->mNumAnimations; ++i) {
		Model::Animation model_animation;
		auto animation = scene->mAnimations[i];
		std::cout << std::endl << "Animation has " << animation->mTicksPerSecond << " ticks per second and is " << animation->mDuration << " ticks long" << std::endl;
		std::cout << "it has " << animation->mNumChannels << " channels" << std::endl;

		model_animation.ticks_per_second = animation->mTicksPerSecond;
		model_animation.duration = animation->mDuration;

		for (size_t c = 0; c < animation->mNumChannels; ++c) {
			Model::Animation::Node animation_node;
			auto node = animation->mChannels[c];
			std::cout << std::endl << "Node is named " << node->mNodeName.C_Str() << std::endl;

			animation_node.name = node->mNodeName.C_Str();

			std::cout << "it has " << node->mNumPositionKeys << " position keys" << std::endl;
			for (size_t p = 0; p < node->mNumPositionKeys; ++p) {
				auto key = node->mPositionKeys[p];
				std::cout << " [t = " << key.mTime << "] " << key.mValue << std::endl;
				animation_node.positions.push_back(
					std::pair<float, glm::vec3>(key.mTime, assimp_to_glm(key.mValue))
				);
			}

			std::cout << "it has " << node->mNumRotationKeys << " rotation keys" << std::endl;
			for (size_t r = 0; r < node->mNumRotationKeys; ++r) {
				auto key = node->mRotationKeys[r];
				std::cout << " [t = " << key.mTime << "] " << key.mValue << std::endl;
				animation_node.rotations.push_back(
					std::pair<float, glm::quat>(key.mTime, assimp_to_glm(key.mValue))
				);
			}

			std::cout << "it has " << node->mNumScalingKeys << " scaling keys" << std::endl;
			for (size_t s = 0; s < node->mNumScalingKeys; ++s) {
				auto key = node->mScalingKeys[s];
				std::cout << " [t = " << key.mTime << "] " << key.mValue << std::endl;
			}

			model_animation.nodes[node->mNodeName.C_Str()] = animation_node;
		}

		model.animations.push_back(model_animation);
	}

	std::cout << std::endl << "Model has " << scene->mNumMeshes << " meshes" << std::endl;
	for (size_t i = 0; i < scene->mNumMeshes; ++i) {
		Model::Mesh node_mesh;
		auto mesh = scene->mMeshes[i];
		std::cout << std::endl << "Mesh is called " << mesh->mName.C_Str() << std::endl;
		std::cout << "it has " << mesh->mNumBones << " bones" << std::endl;

		node_mesh.num_bones = mesh->mNumBones;

		for (size_t j = 0; j < mesh->mNumVertices; ++j) {
			Model::Mesh::Vertex vertex;
			vertex.position = assimp_to_glm(mesh->mVertices[j]);
			// std::cout << "Vertex " << j << " has position " << vertex.position << std::endl;
			node_mesh.vertices.push_back(vertex);
		}

		// Indices
		node_mesh.indices.reserve(mesh->mNumFaces * 3);
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			node_mesh.indices.push_back(mesh->mFaces[j].mIndices[0]);
			node_mesh.indices.push_back(mesh->mFaces[j].mIndices[1]);
			node_mesh.indices.push_back(mesh->mFaces[j].mIndices[2]);
		}

		for (size_t b = 0; b < mesh->mNumBones; ++b) {
			auto bone = mesh->mBones[b];
			std::string bone_name (bone->mName.C_Str());
			std::cout << std::endl << "Bone is called " << bone_name << std::endl;
			std::cout << "its offset matrix is:" << std::endl;
			print_matrix(assimp_to_glm(bone->mOffsetMatrix));
			std::cout << "it has " << bone->mNumWeights << " weights" << std::endl;

			size_t bone_id = 0;
			if (node_mesh.bone_map.find(bone_name) == node_mesh.bone_map.end()) {
				std::cout << "------------------ Bone map size " << node_mesh.bone_map.size() << std::endl;
				bone_id = node_mesh.bone_map.size();

				node_mesh.bone_map[bone_name] = bone_id;

				Model::Mesh::BoneInfo bone_info;
				bone_info.offset = assimp_to_glm(bone->mOffsetMatrix);
				node_mesh.bone_info.push_back(bone_info);
			}

			for (size_t w = 0; w < bone->mNumWeights; ++w) {
				auto vertex_weight = bone->mWeights[w];
				// std::cout << " vertex ID " << bone->mWeights[w].mVertexId << " has weight " << bone->mWeights[w].mWeight << std::endl;
				node_mesh.vertices[vertex_weight.mVertexId].weights[bone_id] = vertex_weight.mWeight;
			}
		}

		node_mesh.model = &model;
		model.meshes.push_back(node_mesh);
	}

	return model;
}

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
		exit(1);
		return 0;
	}

	return shader;
}

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)) {
		std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

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

	Model model_cube = import_model("../res/lock.dae");

	std::cout << "COMPILING STUFF!" << std::endl;
	GLuint vertex_shader = compileShader("../shaders/main.vertex", GL_VERTEX_SHADER);
	GLuint fragment_shader = compileShader("../shaders/main.fragment", GL_FRAGMENT_SHADER);
	GLuint program_main = glCreateProgram();
	glAttachShader(program_main, vertex_shader);
	glAttachShader(program_main, fragment_shader);

	glBindFragDataLocation(program_main, 0, "outColor");
	glBindAttribLocation(program_main, 0, "position");
	// glBindAttribLocation(program_main, 1, "normal");
	// glBindAttribLocation(program_main, 2, "texcoord");
	glBindAttribLocation(program_main, 3, "boneIDs");
	glBindAttribLocation(program_main, 4, "weights");

	glLinkProgram(program_main);
	glUseProgram(program_main);

	GLuint BONE_LOCATIONS[20];
	for (int i = 0; i < 20; ++i) {
		std::stringstream ss;
		ss << "bones[" << i << "]";
		BONE_LOCATIONS[i] = glGetUniformLocation(program_main, ss.str().c_str());
	}

	std::vector<glm::vec3> positions;
	std::vector<unsigned> bone_ids;
	std::vector<float> weights;
	for (auto &vertex : model_cube.meshes[0].vertices) {
		positions.push_back(vertex.position);
		size_t i = 0;
		for (auto &vertex_weight : vertex.weights) {
			i++;
			bone_ids.push_back(vertex_weight.first);
			weights.push_back(vertex_weight.second);
		}
		for (; i < 4; ++i) {
			bone_ids.push_back(-1);
			weights.push_back(0);
		}
	}
	size_t positions_size = positions.size()*sizeof(glm::vec3);
	size_t bone_ids_size = bone_ids.size()*sizeof(unsigned);
	size_t weights_size = bone_ids.size()*sizeof(float);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		positions_size + bone_ids_size + weights_size,
		NULL,
		GL_STATIC_DRAW
	);

	glBufferSubData(GL_ARRAY_BUFFER, 0, positions_size, positions.data());
	glBufferSubData(GL_ARRAY_BUFFER, positions_size, bone_ids_size, bone_ids.data());
	glBufferSubData(GL_ARRAY_BUFFER, positions_size + bone_ids_size, weights_size, weights.data());

	std::cout << positions_size << ", " << bone_ids_size << ", " << weights_size << std::endl;
	std::cout << positions.size() << ", " << bone_ids.size() << ", " << weights.size() << std::endl;
	std::cout << "There are " << model_cube.meshes[0].indices.size() << " indices" << std::endl;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribIPointer(3, 4, GL_INT, 0, (void *) (positions_size));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void *) (positions_size + bone_ids_size));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		model_cube.meshes[0].indices.size()*sizeof(unsigned),
		model_cube.meshes[0].indices.data(),
		GL_STATIC_DRAW
	);

	glViewport(0, 0, WIDTH, HEIGHT);

	glClearColor(0.5, 0.5, 0.5, 1);

	glm::vec3 player_position {0, 0, 5};

	GLuint uniProj = glGetUniformLocation(program_main, "projection");
	GLuint uniView = glGetUniformLocation(program_main, "view");
	glm::mat4 projection = glm::perspective(1.4f, static_cast<float>(WIDTH)/HEIGHT, 0.1f, 100.f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	bool is_running = true;
	float dt = 0;
	float game_time = 0;
	Uint32 ticks = 0;
	while (is_running) {
		Uint32 new_ticks = SDL_GetTicks();
		dt = (new_ticks - ticks) / 1000.f;
		ticks = new_ticks;
		game_time += dt;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				is_running = false;
				break;
			}
		}

		glm::mat4 view;
		static float i = 0;
		i += 0.01;
		view = glm::translate(view, -player_position);
		// view = glm::rotate(view, i, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

		for (auto &mesh : model_cube.meshes) {
			auto transforms = mesh.get_transforms(game_time);
			std::cout << "Got " << transforms.size() << " transforms!" << std::endl;
			for (int i = 0; i < transforms.size(); ++i) {
				std::cout << "Matrix:" << std::endl;
				print_matrix(transforms[i]);
				glUniformMatrix4fv(BONE_LOCATIONS[i], 1, GL_FALSE, glm::value_ptr(transforms[i]));
			}
		}
		/*
		*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, model_cube.meshes[0].indices.size(), GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}
}
