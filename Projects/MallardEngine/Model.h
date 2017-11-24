#pragma once
#include "DLLBuild.h"
#include "IResource.h"
#include "IRenderable.h"

#include <vector>

class Mesh;
class Texture;

struct aiScene;
struct aiNode;

class DLL_BUILD Model : public IResource, public IRenderable {
public:
	Model();
	~Model();

	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	///IResource
	virtual unsigned int getResourceType() const override;

	//TODO: Move this back to private, temp change for instance rendering
	std::vector<Mesh*> m_Meshs;
private:
	///IResource
	virtual bool resourceLoad() override;
	virtual void resourceCopy(IResource * a_Resource) override;
	virtual void resourceUnload() override;
	virtual IResource* resourceCreate() override;

	///model
	void loadNode(aiNode* a_Node);
	void loadTextures();

	///data
	struct ModelMeshData {
		Texture* m_Texture;
		glm::vec3 m_Color;
	};
	std::vector<ModelMeshData*> m_ModelMeshData;

	const aiScene* m_Scene = nullptr;
};

