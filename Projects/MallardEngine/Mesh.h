#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;

struct DLL_BUILD Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class DLL_BUILD Mesh : public IRenderable {
private:
	typedef Vertex VerticesType;
	typedef unsigned int IndicesType;
public:
	Mesh();
	~Mesh();

	// Inherited via IRenderable
	virtual void draw() override;

	void applyData(std::vector<VerticesType> a_Verts, std::vector<IndicesType> a_Indices);
	void loadFromMesh(aiMesh* a_Mesh);

	void bind();

private:
	std::vector<VerticesType> m_Vertices;
	std::vector<IndicesType> m_Indices;
	//textures?

	unsigned int m_Vao, m_Vbo, m_Ebo;
};

