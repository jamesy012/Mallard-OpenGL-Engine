#include "Texture.h"

#include <iostream>

#include <GL\glew.h>
//#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if _DEBUG
#include "Framebuffer.h"
#endif // _DEBUG

#include "Shader.h"
#include "ShaderUniformData.h"

Texture::Texture() {
	m_TextureHeight = m_TextureWidth = 0;
	m_TextureType = TextureType::NONE;
	//todo this might need to change, if i allow texture id's to be set later
	m_CreatedTexture = true;
}

Texture::Texture(unsigned int a_TextureID, unsigned int a_Width, unsigned int a_Height) {
	m_TextureType = TextureType::NONE;
	m_TextureId = a_TextureID;
	m_TextureWidth = a_Width;
	m_TextureHeight = a_Height;
	m_CreatedTexture = false;
}

Texture::Texture(unsigned int a_Width, unsigned int a_Height, TextureType a_Type) {
	m_TextureType = a_Type;
	m_TextureWidth = a_Width;
	m_TextureHeight = a_Height;
	int dataSize = getDataSize();
	if (dataSize == 0) {
		m_CreatedTexture = false;
		return;
	}
	int imageSize = a_Width * a_Height;
	m_TextureData = new GLubyte[imageSize * dataSize]{ 0 };

	m_CreatedTexture = true;
}

Texture::Texture(const Texture & a_Texture) {
	m_TextureType = a_Texture.m_TextureType;
	m_TextureWidth = a_Texture.m_TextureWidth;
	m_TextureHeight = a_Texture.m_TextureHeight;

	m_CreatedTexture = true;

	int dataSize = getDataSize();
	int imageSize = m_TextureWidth * m_TextureHeight;
	m_TextureData = new DataFormat[imageSize*dataSize];
	memcpy(m_TextureData, a_Texture.m_TextureData, (imageSize * dataSize) * sizeof(DataFormat));
}


Texture::~Texture() {
	if (m_CreatedTexture) {
		if (m_TextureId != 0) {
			glDeleteTextures(1, &m_TextureId);
			m_TextureId = 0;
		}
		m_TextureWidth = m_TextureHeight = 0;
		stbi_image_free(m_TextureData);
	}
}

DLL_BUILD void Texture::load1x1Texture() {
	//is there a better way to do this??
	m_TextureData = new GLubyte[3];
	DataFormat data[] = { 255,255,255 };
	memcpy(m_TextureData, &data, 3 * sizeof(DataFormat));


	m_TextureType = TextureType::RGB;

	m_TextureWidth = m_TextureHeight = 1;

	bindTexture();

	m_Resource_LoadOveride = true;
	//load("TEXTURE_1X1");
}

DLL_BUILD unsigned int Texture::getGLTypeFromTextureType(const TextureType a_Type) {
	switch (a_Type) {
		case TextureType::RGB:
			return GL_RGB;
		case TextureType::RGBA:
			return GL_RGBA;
		default:
			return GL_RGB;
	}
}

DLL_BUILD void Texture::bindTexture(const Texture * a_Texture, const int a_Slot) {
	glActiveTexture(GL_TEXTURE0 + a_Slot);
	if (a_Texture == nullptr) {
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}

#if _DEBUG
	Framebuffer* m_Fb = Framebuffer::getCurrentFramebuffer();
	if (m_Fb != nullptr) {
		Texture* framebufferTexture = m_Fb->getTexture();
		if (framebufferTexture != nullptr && framebufferTexture->m_TextureId == a_Texture->m_TextureId) {
			_ASSERT_EXPR(false, L"Binded texture is being used in the current framebuffer");
		}
	}
#endif // DEBUG

	glBindTexture(GL_TEXTURE_2D, a_Texture->m_TextureId);
}

DLL_BUILD void Texture::bindAndApplyTexture(const Texture * a_Texture, const int a_Slot, ShaderUniformData * a_TextureUniform) {
	//bind a_Texture to a_Slot in the current shader
	bindTexture(a_Texture, a_Slot);

	//set a_TextureUniform to the slot we binded the texture to
	a_TextureUniform->setData(&a_Slot);
	//and send it to the uniform
	Shader::applyUniform(a_TextureUniform);
}

DLL_BUILD void Texture::setPixel(unsigned int a_X, unsigned int a_Y, glm::vec4 a_Color) {
	int dataSize = getDataSize();
	int pixelPos = (a_X + (a_Y * m_TextureWidth)) * dataSize;

	//could replace with a memcpy
	m_TextureData[pixelPos + 0] = a_Color.r * 255;
	m_TextureData[pixelPos + 1] = a_Color.g * 255;
	m_TextureData[pixelPos + 2] = a_Color.b * 255;
	m_TextureData[pixelPos + 3] = a_Color.a * 255;
}

DLL_BUILD glm::vec4 Texture::getPixel(unsigned int a_X, unsigned int a_Y) const {

	int dataSize = getDataSize();
	int pixelPos = (a_X + (a_Y * m_TextureWidth)) * dataSize;

	glm::vec4 color;

	//could replace with a memcpy
	color.r = m_TextureData[pixelPos + 0] / 255.0f;
	color.g = m_TextureData[pixelPos + 1] / 255.0f;
	color.b = m_TextureData[pixelPos + 2] / 255.0f;
	if (m_TextureType == TextureType::RGBA) {
		color.a = m_TextureData[pixelPos + 3] / 255.0f;
	} else {
		color.a = 1.0f;
	}
	return color;
}

unsigned int Texture::getResourceType() const {
	return ResourceTypes::RESOUCE_TEXTURE;
}

bool Texture::resourceLoad() {
	//loading has already been completed before this
	if (m_Resource_LoadOveride) {
		return true;
	}
	//stbi image format
	int imageFormat;

	m_TextureData = stbi_load(m_Resource_FileName.c_str(),
							  &m_TextureWidth, &m_TextureHeight, &imageFormat, STBI_default);

	if (m_TextureData == nullptr || m_TextureData == '\0') {
		printf("Could not load Image: %s\n", m_Resource_FileName.c_str());
		return false;
	}

	switch (imageFormat) {
		case STBI_rgb_alpha:
			m_TextureType = TextureType::RGBA;
			break;
		case STBI_grey:
		case STBI_rgb:
			m_TextureType = TextureType::RGB;
			break;
		default:
			m_TextureType = TextureType::RGB;
			break;
	}

	//bind texture
	//bindTexture();

	//free texture
	//stbi_image_free(data);
	return true;
}

void Texture::resourceCopy(IResource * a_Resource) {
	Texture* tex = (Texture*) a_Resource;
	m_TextureWidth = tex->m_TextureWidth;
	m_TextureHeight = tex->m_TextureHeight;
	m_TextureType = tex->m_TextureType;


	int dataSize = getDataSize();

	int textureSize = m_TextureHeight * m_TextureWidth;
	int totalSize = textureSize * dataSize;
	m_TextureData = new DataFormat[totalSize];
	memcpy(m_TextureData, tex->m_TextureData, totalSize * sizeof(DataFormat));
	bindTexture();
}

void Texture::resourceUnload() {
}

DLL_BUILD IResource * Texture::resourceCreate() {
	return new Texture();
}

void Texture::bindTexture() {
	if (m_TextureId != 0) {
		//display warning?, overwriting data
		glDeleteTextures(1, &m_TextureId);
	}

	//get type
	int texTypeGl = getGLTypeFromTextureType(m_TextureType);

	//gen/bind texture
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, texTypeGl, m_TextureWidth, m_TextureHeight, 0, texTypeGl, GL_UNSIGNED_BYTE, m_TextureData);

	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
}

unsigned int Texture::getDataSize() const {
	switch (m_TextureType) {
		case RGBA:
			return 4;
		case RGB:
			return 3;
		default:
			return 0;
	}
}

