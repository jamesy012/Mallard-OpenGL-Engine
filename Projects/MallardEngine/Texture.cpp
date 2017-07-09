#include "Texture.h"

#include <iostream>

#include <GL\glew.h>
//#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture::Texture() {
	std::cout << "TEXTURE CREATE\n";

	m_TextureHeight = m_TextureWidth = 0;
	m_TextureType = TextureType::None;
}


Texture::~Texture() {
	std::cout << "TEXTURE DECONSTUCTOR\n";

	if (m_TextureId != 0) {
		glDeleteTextures(1, &m_TextureId);
		m_TextureId = 0;
	}
	m_TextureWidth = m_TextureHeight = 0;
}

DLL_BUILD void Texture::load1x1Texture() {
	GLubyte data[] = { 0,0,255};
	m_TextureType = TextureType::RGB;
	
	m_TextureWidth = m_TextureHeight = 1;

	bindTexture(data);

	m_Resource_LoadOveride = true;
	load("TEXTURE_1X1");
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

DLL_BUILD void Texture::bindTexture(int a_Slot) {
	//m_TextureId can be 0, and it will just unbind that texture

	glActiveTexture(GL_TEXTURE0 + a_Slot);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

unsigned int Texture::getResourceType() const {
	return ResourceTypes::RESOUCE_TEXTURE;
}

void Texture::resourceLoad() {
	//loading has already been completed before this
	if (m_Resource_LoadOveride) {
		return;
	}
	std::cout << "TEXTURE LOAD\n";

	//stbi image format
	int imageFormat;

	GLubyte* data = stbi_load(m_Resource_FileName.c_str(),
									&m_TextureWidth, &m_TextureHeight, &imageFormat, STBI_default);

	if (data == nullptr || data == '\0') {
		printf("Could not load Image: %s\n", m_Resource_FileName.c_str());
		return;
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
	bindTexture(data);

	//free texture
	stbi_image_free(data);
}

void Texture::resourceCopy(IResource * a_Resource) {
	std::cout << "TEXTURE COPY\n";
}

void Texture::resourceUnload() {
	std::cout << "TEXTURE UNLOAD\n";
}

void Texture::bindTexture(const unsigned char * a_Data) {
	if (m_TextureId != 0) {
		//display warning?, overwriting data
		glDeleteTextures(1, &m_TextureId);
	}

	//get type
	int texTypeGl = getGLTypeFromTextureType(m_TextureType);

	//gen/bind texture
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, texTypeGl, m_TextureWidth, m_TextureHeight, 0, texTypeGl, GL_UNSIGNED_BYTE, a_Data);

	//filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

