#include <GLModel.hpp>

GLModel::GLModel(Model model)
{
	for (auto &mesh : model.meshes) {
		meshes.push_back(std::unique_ptr<GLMesh>(new GLMesh(mesh, model.textures)));
	}
}

void GLModel::render() const
{
	for (auto &mesh : meshes) {
		mesh->render();
	}
}
