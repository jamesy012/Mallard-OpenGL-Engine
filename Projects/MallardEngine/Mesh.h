#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
};

class  Mesh : public IRenderable {
private:
	typedef Vertex VerticesType;
	typedef unsigned int IndicesType;
public:
	DLL_BUILD Mesh();
	DLL_BUILD ~Mesh();

	//creates a box for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	DLL_BUILD void createBox();

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
