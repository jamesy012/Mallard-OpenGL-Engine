#pragma once

#include "DLLBuild.h"

#include "IResource.h"

enum TextureType {
	NONE,
	RGB,
	RGBA
};

class ShaderUniformData;

class  Texture : public IResource {
private:
	typedef unsigned char DataFormat;
public:
	DLL_BUILD Texture();
	DLL_BUILD Texture(unsigned int a_TextureID,int a_Width,int a_Height);
	DLL_BUILD ~Texture();

	//todo add color parameter
	DLL_BUILD void load1x1Texture();

	DLL_BUILD static unsigned int getGLTypeFromTextureType(const TextureType a_Type);
	//binds this texture to the slot in a_Slot
	//DLL_BUILD void bindTexture(const int a_Slot) const;

	DLL_BUILD static void bindTexture(const Texture* a_Texture, const int a_Slot);
	DLL_BUILD static void bindAndApplyTexture(const Texture* a_Texture, const int a_Slot, ShaderUniformData * a_TextureUniform);
	//DLL_BUILD static void bindAndApplyTexture(const unsigned int a_Texture, const int a_Slot, ShaderUniformData * a_TextureUniform);

	///IResource
	DLL_BUILD virtual unsigned int getResourceType() const override;
private:
	///IResource
	DLL_BUILD virtual bool resourceLoad() override;
	DLL_BUILD virtual void resourceCopy(IResource * a_Resource) override;
	DLL_BUILD virtual void resourceUnload() override;
	DLL_BUILD virtual IResource* resourceCreate() override;

	///texture
	void bindTexture();


	///data
	//the type of this texture
	//use this to get the strides for the m_TextureData
	TextureType m_TextureType;
	int m_TextureWidth;
	int m_TextureHeight;
	//Id for the texture it's referencing on the gpu/OpenGL
	unsigned int m_TextureId;

	//this is the data to the texture
	//use TextureType to get the strides/type of data this is
	DataFormat* m_TextureData;

	//this tells the class weather it loaded and created the texture ID
	//will be true if file loaded by string
	//will be false if a texture ID was given to it (IE: the constructor)
	bool m_CreatedTexture;

};

