#pragma once

#include "DLLBuild.h"

#include "IResource.h"

class DLL_BUILD Texture : public IResource {
public:
	Texture();
	~Texture();


	// Inherited via IResource
	virtual unsigned int getResourceType() const override;
private:
	virtual void resourceLoad() override;

	virtual void resourceCopy(IResource * a_Resource) override;

	virtual void resourceUnload() override;

};

