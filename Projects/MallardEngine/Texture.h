#pragma once

#include "DLLBuild.h"

#include "IResource.h"

#include <glm\glm.hpp>

enum TextureType {
	NONE,
	RGB,
	RGBA
};

class ShaderUniformData;
class TexturePacker;


class DLL_BUILD Texture : public IResource {
private:
	friend TexturePacker;
	typedef unsigned char DataFormat;
public:
	//create a simple texture,
	//should load a texture from here
	Texture();
	//provide a textureID and size of a texture
	//use this if the texture was created elsewhere(Eg. Framebuffer)
	Texture(unsigned int a_TextureID, unsigned int a_Width, unsigned int a_Height);
	//use this to create a texture with it's texture data set up
	//this allows you to modify the textues pixels
	Texture(unsigned int a_Width, unsigned int a_Height, TextureType a_Type);
	//copys texture
	Texture(const Texture& a_Texture);
	~Texture();

	//todo add color parameter
	//void load1x1Texture();

	static unsigned int getGLTypeFromTextureType(const TextureType a_Type);
	//binds this texture to the slot in a_Slot
	//void bindTexture(const int a_Slot) const;

	//binds this texture ID to the shader uniform slot in a_Slot
	static void bindTexture(const Texture* a_Texture, const int a_Slot);
	//binds this texture to the Shader slot and applys the slot to a_TextureUniform;
	static void bindAndApplyTexture(const Texture* a_Texture, const int a_Slot, ShaderUniformData * a_TextureUniform);
	//static void bindAndApplyTexture(const unsigned int a_Texture, const int a_Slot, ShaderUniformData * a_TextureUniform);

	//binds the texture to opengl, getting the textureID and making this texture useable
	void bind();

	//changes the local stored version of the image
	//will need to bind after changing to update the image
	void setPixel(unsigned int a_X, unsigned int a_Y, glm::vec4 a_Color);
	//changes the local stored version of the image
	//will need to bind after changing to update the image
	//will not change the alpha
	void setPixel(unsigned int a_X, unsigned int a_Y, glm::vec3 a_Color);
	glm::vec4 getPixel(unsigned int a_X, unsigned int a_Y) const;

	///IResource
	virtual unsigned int getResourceType() const override;

	//simple common 1x1 texture, handled by Application.cpp
	static Texture* m_White1x1Texture;
private:
	///IResource
	virtual bool resourceLoad() override;
	virtual void resourceCopy(IResource * a_Resource) override;
	virtual void resourceUnload() override;
	virtual IResource* resourceCreate() override;



	unsigned int getDataSize()const;

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

