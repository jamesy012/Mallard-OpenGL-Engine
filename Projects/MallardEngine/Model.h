#pragma once
#include "DLLBuild.h"
#include "IResource.h"
#include "IRenderable.h"

#include <vector>

class Mesh;

struct aiScene;
struct aiNode;

class  Model : public IResource, public IRenderable {
public:
	Model();
	~Model();

	DLL_BUILD virtual unsigned int getResourceType() const override;
	DLL_BUILD virtual void draw() override;

private:
	DLL_BUILD void loadNode(aiNode* a_Node);

	DLL_BUILD virtual void resourceLoad() override;
	DLL_BUILD virtual void resourceCopy(IResource * a_Resource) override;
	DLL_BUILD virtual void resourceUnload() override;

	std::vector<Mesh*> m_Meshs;

	const aiScene* m_Scene = nullptr;
};

