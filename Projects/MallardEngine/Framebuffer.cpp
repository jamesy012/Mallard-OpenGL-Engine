#include "Framebuffer.h"

#include <GL\glew.h>
#include <stdio.h>

#include "Texture.h"
#include "Window.h"

static Framebuffer* m_DefaultFramebuffer = nullptr;
static Framebuffer* m_CurrentFramebuffer = nullptr;

Framebuffer::Framebuffer() {
	m_Fbo = 0;
	m_Width = m_Height = 1;//1x1 starting size
}


Framebuffer::~Framebuffer() {
	if (m_Fbo != 0) {
		glDeleteFramebuffers(1, &m_Fbo);
		m_Fbo = 0;
	}
	//go through all attached components and delete their opengl info
	for (unsigned int i = 0; i < m_AttachedComponents.size(); i++) {
		Component* component = m_AttachedComponents[i];
		switch (component->m_Type) {
			case FramebufferBufferTypes::TEXTURE:
				//convert component into FramebufferTexture, then get the address of m_TextureID and m_TextureObject
				glDeleteTextures(1, &((FramebufferTexture*)component)->m_TextureID);
				delete ((FramebufferTexture*)component)->m_TextureObject;
				break;
			case FramebufferBufferTypes::RENDERBUFFER:
				//convert component into FramebufferRenderbuffer, then get the address of m_RenderbufferID
				glDeleteRenderbuffers(1, &((FramebufferRenderbuffer*)component)->m_RenderbufferID);
				break;
		}
		//finaly delete the component
		delete component;
	}
	//clear out all other arrays using those components
	m_AttachedComponents.clear();
	m_ColorAttachments.clear();
	m_OtherAttachments.clear();
}

void Framebuffer::setSize(const unsigned int a_Width, const unsigned int a_Height) {
	if (m_Fbo != 0) {
		resizeFramebuffer(a_Width, a_Height);
	} else {
		m_Width = a_Width;
		m_Height = a_Height;
	}
}

void Framebuffer::use(Framebuffer * a_Framebuffer) {
	//if we are setting the current framebuffer to nullprt or 0
	if (a_Framebuffer == nullptr || a_Framebuffer->m_Fbo == 0) {
		//then check what if the default framebuffer is null or 0
		//if it is then render to fbo 0 (the window)
		if (m_DefaultFramebuffer == nullptr || m_DefaultFramebuffer->m_Fbo == 0) {
			m_CurrentFramebuffer = m_DefaultFramebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			Window* window = Window::getMainWindow();
			glViewport(0, 0, window->getFramebufferWidth(), window->getFramebufferHeight());
			return;
		} else {
			//else render to the default framebuffer
			a_Framebuffer = m_DefaultFramebuffer;
		}
	}
	m_CurrentFramebuffer = a_Framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, a_Framebuffer->m_Fbo);
	glViewport(0, 0, a_Framebuffer->m_Width, a_Framebuffer->m_Height);
}

void Framebuffer::clearCurrentBuffer(bool a_ColorBit, bool a_DepthBit) {
	int clearBit = 0;
	if (a_DepthBit) {
		clearBit |= GL_DEPTH_BUFFER_BIT;
	}
	if (a_ColorBit) {
		clearBit |= GL_COLOR_BUFFER_BIT;
	}
	glClear(clearBit);
}

void Framebuffer::setDefaultFramebuffer(Framebuffer * a_Framebuffer) {
	m_DefaultFramebuffer = a_Framebuffer;
	use(a_Framebuffer);
}

Framebuffer * Framebuffer::getCurrentFramebuffer() {
	return m_CurrentFramebuffer;
}

void Framebuffer::framebufferBlit(const Framebuffer * a_From, const Framebuffer * a_To) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, a_From->m_Fbo);
	if (a_To == nullptr) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, a_From->m_Width, a_From->m_Height, 0, 0, Window::getMainWindow()->getFramebufferWidth(), Window::getMainWindow()->getFramebufferHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	} else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, a_To->m_Fbo);

		glBlitFramebuffer(0, 0, a_From->m_Width, a_From->m_Height, 0, 0, a_To->m_Width, a_To->m_Height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	//if (m_CurrentFramebuffer == nullptr) {
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//} else {
	//	glBindFramebuffer(GL_FRAMEBUFFER, m_CurrentFramebuffer->m_Fbo);
	//}
}

void Framebuffer::resizeFramebuffer(unsigned int a_Width, unsigned int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
	for (unsigned int i = 0; i < m_AttachedComponents.size(); i++) {
		Component* component = m_AttachedComponents[i];
		unsigned int glFormat = getGLFormatSize(component->m_Format, component->m_FormatSize);
		switch (component->m_Type) {
			case FramebufferBufferTypes::TEXTURE:
				{
					FramebufferTexture* ft = (FramebufferTexture*)component;
					glBindTexture(GL_TEXTURE_2D, ft->m_TextureID);
					glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_Width, m_Height, 0, ft->m_BaseGLFormat, GL_FLOAT, 0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				break;
			case FramebufferBufferTypes::RENDERBUFFER:
				{
					FramebufferRenderbuffer* fr = (FramebufferRenderbuffer*) component;
					glBindRenderbuffer(GL_RENDERBUFFER, fr->m_RenderbufferID);
					glRenderbufferStorage(GL_RENDERBUFFER, glFormat, m_Width, m_Height);
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
				}
				break;
		}
	}
}

void Framebuffer::genFramebuffer() {
	//create frame buffer
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	//add normal components
	for (unsigned int i = 0; i < m_OtherAttachments.size(); i++) {
		Component* component = m_OtherAttachments[i];
		switch (component->m_Type) {
			case FramebufferBufferTypes::TEXTURE:
				glFramebufferTexture(GL_FRAMEBUFFER, getGLAttachment(component->m_Format), ((FramebufferTexture*)component)->m_TextureID, 0);
				break;
			case FramebufferBufferTypes::RENDERBUFFER:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, getGLAttachment(component->m_Format), GL_RENDERBUFFER, ((FramebufferRenderbuffer*)component)->m_RenderbufferID);
				break;
		}
	}

	//add color attachments
	std::vector<unsigned int> drawBuffers;

	for (unsigned int i = 0; i < m_ColorAttachments.size(); i++) {
		Component* component = m_ColorAttachments[i];
		//note: this can go above the max amount of color attachments
		unsigned int colorAttachment = GL_COLOR_ATTACHMENT0 + i;
		//add this color attachment to the drawBuffers
		drawBuffers.push_back(colorAttachment);
		//and apply the component to the framebuffer
		switch (component->m_Type) {
			case FramebufferBufferTypes::TEXTURE:
				glFramebufferTexture(GL_FRAMEBUFFER, colorAttachment, ((FramebufferTexture*)component)->m_TextureID, 0);
				break;
			case FramebufferBufferTypes::RENDERBUFFER:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, colorAttachment, GL_RENDERBUFFER, ((FramebufferRenderbuffer*)component)->m_RenderbufferID);
				break;
		}

	}

	//color attachments
	glDrawBuffers(m_ColorAttachments.size(), drawBuffers.data());

	//simple error checker
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error ID: %u\n", status);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//done, lets unbind the buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Framebuffer::createRenderTarget() {
	addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA, 16);
	addBuffer(FramebufferBufferTypes::RENDERBUFFER, FramebufferBufferFormats::DEPTH, 16);
	genFramebuffer();
}


void Framebuffer::addBuffer(FramebufferBufferTypes a_Type, FramebufferBufferFormats a_Format, unsigned int a_FormatSize) {
	unsigned int glFormat = getGLFormatSize(a_Format, a_FormatSize);
	Component* component = nullptr;
	switch (a_Type) {
		case FramebufferBufferTypes::TEXTURE:
		{
			unsigned int baseGLFormat = getGLFormat(a_Format);
			unsigned int textureID = 0;
			//create texture
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_Width, m_Height);
			glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_Width, m_Height, 0, baseGLFormat, GL_UNSIGNED_BYTE, 0);

			//basic texture filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			GLfloat borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			//unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			//create TextureObject
			Texture* textureObject = new Texture(textureID, m_Width, m_Height);

			//set up component
			component = new FramebufferTexture();
			((FramebufferTexture*)component)->m_TextureID = textureID;
			((FramebufferTexture*)component)->m_TextureObject = textureObject;
			((FramebufferTexture*)component)->m_BaseGLFormat = baseGLFormat;

			m_Textures.push_back(textureObject);
			break;
		}

		case FramebufferBufferTypes::RENDERBUFFER:
		{
			unsigned int renderBuffer = 0;

			//create renderbuffer
			glGenRenderbuffers(1, &renderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
			//set up renderbuffer
			glRenderbufferStorage(GL_RENDERBUFFER, glFormat, m_Width, m_Height);

			//unbind renderbuffer
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			//set up component
			component = new FramebufferRenderbuffer();
			((FramebufferRenderbuffer*)component)->m_RenderbufferID = renderBuffer;

			break;
		}
		default:
			return;
	}
	component->m_Type = a_Type;
	component->m_Format = a_Format;
	component->m_FormatSize = a_FormatSize;

	//add component to vectors
	switch (a_Format) {
		case FramebufferBufferFormats::R:
		case FramebufferBufferFormats::RG:
		case FramebufferBufferFormats::RGB:
		case FramebufferBufferFormats::RGBA:
			//add R,RG,RGB,RGBA to color attachments and to attached components
			m_ColorAttachments.push_back(component);
			break;
		default:
			//add everything else to this array
			m_OtherAttachments.push_back(component);
			break;
	}
	//add everything into attached components
	m_AttachedComponents.push_back(component);

}

void Framebuffer::addBuffer(FramebufferBufferTypes a_Type, FramebufferBufferFormats a_Format) {

	switch (a_Format) {
		case FramebufferBufferFormats::R:
		case FramebufferBufferFormats::RG:
		case FramebufferBufferFormats::RGB:
		case FramebufferBufferFormats::RGBA:
			addBuffer(a_Type, a_Format, 8);
			break;
		case FramebufferBufferFormats::DEPTH:
			addBuffer(a_Type, a_Format, 16);
			break;
		default:
			//size is not used with this format
			addBuffer(a_Type, a_Format, 0);
			break;
	}

}

Texture * Framebuffer::getTexture(const unsigned int a_TextureIndex) const {
	if (m_Textures.size() <= a_TextureIndex) {
		return nullptr;
	}
	return m_Textures[a_TextureIndex];
}

unsigned int Framebuffer::getGLCallFromEnum(GL_CALLS a_Call) {
	switch (a_Call) {
		case Framebuffer::GL_CALLS::DEPTH_TEST:
			return GL_DEPTH_TEST;
		case Framebuffer::GL_CALLS::CULL_FACE:
			return GL_CULL_FACE;
		default:
			return 0;
	}
}

void Framebuffer::glCall(GL_CALLS a_Call, bool a_Enabled) {
	unsigned int glCall = getGLCallFromEnum(a_Call);
	if (a_Enabled) {
		glEnable(glCall);
	} else {
		glDisable(glCall);
	}
}

//converts a_Format and a_FormatSize into their opengl counterparts
//eg a_Format = RGB, a_FormatSize = 16
//result GL_RGB16 (0x8054)
unsigned int Framebuffer::getGLFormatSize(FramebufferBufferFormats a_Format, unsigned int a_FormatSize) {
	switch (a_Format) {
		case FramebufferBufferFormats::R:
		{
			switch (a_FormatSize) {
				case 8:
					return GL_R8;
				case 16:
					return GL_R16;
			}
			_ASSERT_EXPR(false, L"FramebufferBufferFormats::R does not have a_FormatSize");
		}
		case FramebufferBufferFormats::RG:
		{
			switch (a_FormatSize) {
				case 8:
					return GL_RG8;
				case 16:
					return GL_RG16;
			}
			_ASSERT_EXPR(false, L"FramebufferBufferFormats::RG does not have a_FormatSize");
		}
		case FramebufferBufferFormats::RGB:
		{
			switch (a_FormatSize) {
				case 4:
					return GL_RGB4;
				case 5:
					return GL_RGB5;
				case 8:
					return GL_RGB8;
				case 10:
					return GL_RGB10;
				case 12:
					return GL_RGB12;
				case 16:
					return GL_RGB16;
			}
			_ASSERT_EXPR(false, L"FramebufferBufferFormats::RGB does not have a_FormatSize");
		}
		case FramebufferBufferFormats::RGBA:
		{
			switch (a_FormatSize) {
				case 2:
					return GL_RGBA2;
				case 4:
					return GL_RGBA4;
				case 8:
					return GL_RGBA8;
				case 12:
					return GL_RGBA12;
				case 16:
					return GL_RGBA16;
			}
			_ASSERT_EXPR(false, L"FramebufferBufferFormats::RGBA does not have a_FormatSize");
		}
		case FramebufferBufferFormats::DEPTH:
		{
			switch (a_FormatSize) {
				case 16:
					return GL_DEPTH_COMPONENT16;
				case 24:
					return GL_DEPTH_COMPONENT24;
				case 32:
					return GL_DEPTH_COMPONENT32;
			}
			_ASSERT_EXPR(false, L"FramebufferBufferFormats::DEPTH does not have a_FormatSize");
		}
		case FramebufferBufferFormats::STENCIL:
			return GL_STENCIL;
		case FramebufferBufferFormats::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL;
	}
	return -1;
}

unsigned int Framebuffer::getGLFormat(FramebufferBufferFormats a_Format) {
	switch (a_Format) {
		case FramebufferBufferFormats::R:
			return GL_R;
		case FramebufferBufferFormats::RG:
			return GL_RG;
		case FramebufferBufferFormats::RGB:
			return GL_RGB;
		case FramebufferBufferFormats::RGBA:
			return GL_RGBA;
		case FramebufferBufferFormats::DEPTH:
			return GL_DEPTH_COMPONENT;
		case FramebufferBufferFormats::STENCIL:
			return GL_STENCIL;
		case FramebufferBufferFormats::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL;

	}
	return -1;
}

unsigned int Framebuffer::getGLAttachment(FramebufferBufferFormats a_Format) {
	switch (a_Format) {
		case FramebufferBufferFormats::R:
		case FramebufferBufferFormats::RG:
		case FramebufferBufferFormats::RGB:
		case FramebufferBufferFormats::RGBA:
			return GL_COLOR_ATTACHMENT0;
		case FramebufferBufferFormats::DEPTH:
			return GL_DEPTH_ATTACHMENT;
		case FramebufferBufferFormats::STENCIL:
			return GL_STENCIL_ATTACHMENT;
		case FramebufferBufferFormats::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL_ATTACHMENT;

	}
	return -1;
}

