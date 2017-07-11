#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;
class Texture;

struct Vertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
};

class DLL_BUILD Mesh : public IRenderable {
private:
	typedef Vertex VerticesType;
	typedef unsigned int IndicesType;
public:
	Mesh();
	~Mesh();
	Mesh(const Mesh& a_Mesh);

	//creates a box for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	void createBox();

	// Inherited via IRenderable
	virtual void draw() override;

	void applyData(std::vector<VerticesType> a_Verts, std::vector<IndicesType> a_Indices);
	void loadFromMesh(aiMesh* a_Mesh);

	void bind();

	void setTexture(Texture* a_Texture);

	//temporary fix 
	//this stores the index of which texture this is using in it's model
	int m_TextureIndex = 0;
private:
	std::vector<VerticesType> m_Vertices;
	std::vector<IndicesType> m_Indices;
	
	//todo change to material
	//test Texture
	Texture* m_Texture = nullptr;

	unsigned int m_Vao, m_Vbo, m_Ebo;
};

