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

	///IResource
	virtual unsigned int getResourceType() const override;
private:
	///IResource
	virtual void resourceLoad() override;
	virtual void resourceCopy(IResource * a_Resource) override;
	virtual void resourceUnload() override;
	virtual IResource* resourceCreate() override;

	///model
	void loadNode(aiNode* a_Node);
	void loadTextures();


	///data
	std::vector<Mesh*> m_Meshs;
	std::vector<Texture*> m_Textures;

	const aiScene* m_Scene = nullptr;
};

