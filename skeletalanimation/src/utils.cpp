#include "utils.hpp"

#include <cstdio>
#include <iostream>

glm::mat4 assimp_to_glm(const aiMatrix4x4& m)
{
	return glm::mat4(
		glm::vec4(m.a1, m.b1, m.c1, m.d1),
		glm::vec4(m.a2, m.b2, m.c2, m.d2),
		glm::vec4(m.a3, m.b3, m.c3, m.d3),
		glm::vec4(m.a4, m.b4, m.c4, m.d4)
	);
}

glm::quat assimp_to_glm(const aiQuaternion& quat)
{
	glm::quat new_quat;
	new_quat.x = quat.x;
	new_quat.y = quat.y;
	new_quat.z = quat.z;
	new_quat.w = quat.w;
	return new_quat;
}

glm::vec3 assimp_to_glm(const aiVector3D& vector)
{
	return glm::vec3 {
		vector.x,
		vector.y,
		vector.z,
	};
}

void print_matrix(const glm::mat4& m)
{
	printf(" %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n",
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	);
}

aiQuaternion glm_to_assimp(const glm::quat &quat)
{
	aiQuaternion new_quat;
	new_quat.x = quat.x;
	new_quat.y = quat.y;
	new_quat.z = quat.z;
	new_quat.w = quat.w;
	return new_quat;
}

std::ostream& operator<< (std::ostream& o, glm::vec3 const& vector)
{
	return o << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
}

std::ostream& operator<< (std::ostream& o, glm::quat const& quat)
{
	return o << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")";
}

std::ostream& operator<< (std::ostream& o, aiVector3D const& vector)
{
	return o << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
}

std::ostream& operator<< (std::ostream& o, aiQuaternion const& quat)
{
	return o << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")";
}
