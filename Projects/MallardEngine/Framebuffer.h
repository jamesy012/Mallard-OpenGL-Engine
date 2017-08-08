#pragma once
#include "DLLBuild.h"

class Texture;

//TESTING WITH HARDCODED RENDER TARGET
class DLL_BUILD Framebuffer {
public:
	Framebuffer();
	~Framebuffer();

	void setSize(const unsigned int a_Width, const unsigned int a_Height);

	static void use(Framebuffer* a_Framebuffer);

	void genFramebuffer();

	//returns a reference to the texture used by this Framebuffer
	//will be nullptr if no framebuffer has been generated
	Texture* getTexture() const;

private:
	unsigned int m_Width;
	unsigned int m_Height;

	//unsigned int m_FboTexture;
	Texture* m_FboTexture;
	unsigned int m_Fbo;
	unsigned int m_RboDepth;
};

