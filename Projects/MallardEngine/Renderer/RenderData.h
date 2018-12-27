#pragma once
#include "DLLBuild.h"

#include "Renderable.h"

#include <string>

#include <vector>

#include <glm\glm.hpp>

struct MeshVertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
};

typedef MeshVertex MeshVerticesType;
typedef unsigned int MeshIndicesType;

class DLL_BUILD RenderData : public Renderable {
protected:
	std::vector<MeshVerticesType> m_Vertices;
	std::vector<MeshIndicesType> m_Indices;

	virtual void bind();

};