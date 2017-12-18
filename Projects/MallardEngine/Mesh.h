#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;
struct aiMaterial;
class Texture;
class MeshBatch;

struct MeshVertex {
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
	glm::vec4 color = glm::vec4(1,1,1,1);
};

typedef MeshVertex MeshVerticesType;
typedef unsigned int MeshIndicesType;

class DLL_BUILD Mesh : public IRenderable {
	friend MeshBatch;
public:
	Mesh();
	~Mesh();
	Mesh(const Mesh& a_Mesh);

	//creates a box for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	void createBox();

	//creates a flat plane facing up for this mesh
	//if there is already a mesh here then it will overwrite the old mesh
	void createPlane(bool a_FlipYUV);

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	void applyData(std::vector<MeshVerticesType> a_Verts, std::vector<MeshIndicesType> a_Indices);
	void loadFromMesh(aiMesh* a_Mesh, aiMaterial* a_Material);

	void bind();

	void setTexture(Texture* a_Texture);

	//temporary fix 
	//this stores the index of which texture this is using in it's model
	int m_TextureIndex = 0;

	//TODO: Move this back to private, temp change for instance rendering
	unsigned int m_Vao;
private:
	std::vector<MeshVerticesType> m_Vertices;
	std::vector<MeshIndicesType> m_Indices;
	
	//todo change to material
	//test Texture
	Texture* m_Texture = nullptr;
	//did this class create a texture for itself?
	bool m_CreatedTexture = false;

	unsigned int m_Vbo, m_Ebo;
};

