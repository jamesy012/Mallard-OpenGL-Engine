#pragma once

#include "DLLBuild.h"

#include "IResource.h"

enum TextureType {
	None,
	RGB,
	RGBA
};

class  Texture : public IResource {
private:
	typedef unsigned char DataFormat;
public:
	DLL_BUILD Texture();
	DLL_BUILD ~Texture();

	//todo add color parameter
	DLL_BUILD void load1x1Texture();

	DLL_BUILD static unsigned int getGLTypeFromTextureType(const TextureType a_Type);
	//binds this texture to the slot in a_Slot
	DLL_BUILD void bindTexture(int a_Slot);

	///IResource
	DLL_BUILD virtual unsigned int getResourceType() const override;
private:
	///IResource
	DLL_BUILD virtual void resourceLoad() override;
	DLL_BUILD virtual void resourceCopy(IResource * a_Resource) override;
	DLL_BUILD virtual void resourceUnload() override;
	DLL_BUILD virtual IResource* resourceCreate() override;

	///texture
	void bindTexture();


	///data
	TextureType m_TextureType;
	int m_TextureWidth;
	int m_TextureHeight;
	unsigned int m_TextureId;
	DataFormat* m_TextureData;

};

