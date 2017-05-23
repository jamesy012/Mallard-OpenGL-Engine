#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class  Mesh : public IRenderable {
private:
	typedef Vertex VerticesType;
	typedef unsigned int IndicesType;
public:
	Mesh();
	~Mesh();

	// Inherited via IRenderable
	DLL_BUILD virtual void draw() override;

	DLL_BUILD void applyData(std::vector<VerticesType> a_Verts, std::vector<IndicesType> a_Indices);
	DLL_BUILD void loadFromMesh(aiMesh* a_Mesh);

	DLL_BUILD void bind();

private:
	std::vector<VerticesType> m_Vertices;
	std::vector<IndicesType> m_Indices;
	//textures?

	unsigned int m_Vao, m_Vbo, m_Ebo;
};

