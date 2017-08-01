#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;
class Texture;

struct MeshVertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
};

typedef MeshVertex MeshVerticesType;
typedef unsigned int MeshIndicesType;

class DLL_BUILD Mesh : public IRenderable {

public:
	Mesh();
	~Mesh();
	Mesh(const Mesh& a_Mesh);

	//creates a box for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	void createBox();

	//creates a flat plane facing up for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	void createPlane();

	// Inherited via IRenderable
	virtual void draw() override;

	void applyData(std::vector<MeshVerticesType> a_Verts, std::vector<MeshIndicesType> a_Indices);
	void loadFromMesh(aiMesh* a_Mesh);

	void bind();

	void setTexture(Texture* a_Texture);

	//temporary fix 
	//this stores the index of which texture this is using in it's model
	int m_TextureIndex = 0;
private:
	std::vector<MeshVerticesType> m_Vertices;
	std::vector<MeshIndicesType> m_Indices;
	
	//todo change to material
	//test Texture
	Texture* m_Texture = nullptr;

	unsigned int m_Vao, m_Vbo, m_Ebo;
};

