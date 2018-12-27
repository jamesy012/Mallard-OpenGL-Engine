#pragma once
#include "DLLBuild.h"
#include "Renderer/RenderData.h"

#include <glm\glm.hpp>

class Mesh;
class Model;
class TexturePacker;

class DLL_BUILD MeshBatch : public RenderData {
public:
	MeshBatch();
	MeshBatch(unsigned int a_TexturePackWidth, unsigned int a_TexturePackHeight);
	MeshBatch(MeshBatch& a_Copy);
	~MeshBatch();

	void setFromBatch(MeshBatch* a_OtherBatch);

	void bind() override;
	void add(Mesh* a_Mesh, glm::mat4 a_ModelMatrix);
	void add(Model* a_Model, glm::mat4 a_ModelMatrix);

	//// Inherited via IRenderable
	//virtual void draw() override;
	//virtual void drawInstance(unsigned int a_Amount) override;

	bool m_BindAfterAdd = false;
	TexturePacker* m_TexturePacked;

	//Mesh* m_CombinedMesh = nullptr;
};

