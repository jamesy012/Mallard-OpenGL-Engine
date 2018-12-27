#pragma once
#include "DLLBuild.h"
#include "Renderer/RenderData.h"

#include <vector>

#include <glm\glm.hpp>

struct aiMesh;
struct aiMaterial;

class Texture;
class MeshBatch;

class DLL_BUILD Mesh : public RenderData {
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

	//DEPRECATED("Will be done via Renderable")
	//	void draw() override;
	//DEPRECATED("Will be done via a inherited Renderable")
	//	void drawInstance(unsigned int a_Amount);

	void applyData(std::vector<MeshVerticesType> a_Verts, std::vector<MeshIndicesType> a_Indices);
	void loadFromMesh(aiMesh* a_Mesh, aiMaterial* a_Material);

	void bind();

	void setTexture(Texture* a_Texture);

	void setDebugName(std::string a_Name);

	unsigned int getVerticesCount();
	glm::vec3 getVertexPosition(const unsigned int a_Index);
	glm::vec3 getClosestPosition(const glm::vec3 a_Position, const float a_MinDist = -1);

	Texture* getTexture() const;

	//temporary fix 
	//this stores the index of which texture this is using in it's model
	int m_TextureIndex = 0;

private:

	//todo change to material
	//test Texture
	Texture* m_Texture = nullptr;
	//did this class create a texture for itself?
	bool m_CreatedTexture = false;

};

