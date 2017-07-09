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
	DLL_BUILD Model();
	DLL_BUILD ~Model();

	DLL_BUILD virtual void draw() override;

	///IResource
	DLL_BUILD virtual unsigned int getResourceType() const override;
private:
	///IResource
	DLL_BUILD virtual void resourceLoad() override;
	DLL_BUILD virtual void resourceCopy(IResource * a_Resource) override;
	DLL_BUILD virtual void resourceUnload() override;

	///model
	DLL_BUILD void loadNode(aiNode* a_Node);

	///data
	std::vector<Mesh*> m_Meshs;

	const aiScene* m_Scene = nullptr;
};

