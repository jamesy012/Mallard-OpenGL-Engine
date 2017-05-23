#pragma once
#include "DLLBuild.h"
#include "IResource.h"
#include "IRenderable.h"

#include <vector>

class Mesh;

struct aiScene;
struct aiNode;

class DLL_BUILD Model : public IResource, public IRenderable {
public:
	Model();
	~Model();

	virtual unsigned int getResourceType() const override;
	virtual void draw() override;

private:
	void loadNode(aiNode* a_Node);

	virtual void resourceLoad() override;
	virtual void resourceCopy(IResource * a_Resource) override;
	virtual void resourceUnload() override;

	std::vector<Mesh*> m_Meshs;

	const aiScene* m_Scene = nullptr;
};

