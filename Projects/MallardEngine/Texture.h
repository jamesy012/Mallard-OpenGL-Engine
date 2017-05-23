#pragma once

#include "DLLBuild.h"

#include "IResource.h"

class  Texture : public IResource {
public:
	DLL_BUILD Texture();
	DLL_BUILD ~Texture();


	// Inherited via IResource
	DLL_BUILD virtual unsigned int getResourceType() const override;
private:
	DLL_BUILD virtual void resourceLoad() override;

	DLL_BUILD virtual void resourceCopy(IResource * a_Resource) override;

	DLL_BUILD virtual void resourceUnload() override;

};

