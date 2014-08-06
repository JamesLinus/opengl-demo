#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>

glm::mat4 assimp_to_glm(const aiMatrix4x4 &matrix);
glm::quat assimp_to_glm(const aiQuaternion &quat);
glm::vec3 assimp_to_glm(const aiVector3D &vector);
void print_matrix(const glm::mat4& m);

aiQuaternion glm_to_assimp(const glm::quat &quat);

std::ostream& operator<< (std::ostream& o, glm::vec3 const& vector);
std::ostream& operator<< (std::ostream& o, glm::quat const& quat);
std::ostream& operator<< (std::ostream& o, aiVector3D const& vector);
std::ostream& operator<< (std::ostream& o, aiQuaternion const& quat);
