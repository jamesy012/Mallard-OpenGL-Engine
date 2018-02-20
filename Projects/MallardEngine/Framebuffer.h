#pragma once
#include "DLLBuild.h"

#include <vector>

class Texture;
class Application;

//types of formats that can be assigned to a texture or renderbuffer
enum class FramebufferBufferFormats {
	R,
	RG,
	RGB,
	RGBA,
	DEPTH,
	STENCIL,
	DEPTH_STENCIL,
};

//types of buffers that can be added to this Framebuffer
//Texture if you want to read it
//RenderBuffer if you dont
enum class FramebufferBufferTypes {
	TEXTURE,
	RENDERBUFFER
};

class DLL_BUILD Framebuffer {
public:
	Framebuffer();
	~Framebuffer();

	void setSize(const unsigned int a_Width, const unsigned int a_Height);

	static void use(Framebuffer* a_Framebuffer);

	//TODO, allow choice between which bits
	static void clearCurrentBuffer(bool a_ColorBit = true, bool a_DepthBit = true);

	//Sets the default Framebuffer.
	//The default frame buffer is used
	static void setDefaultFramebuffer(Framebuffer* a_Framebuffer);

	static Framebuffer* getCurrentFramebuffer();

	//copys the texture from a_From to a_To
	//Keeps the same binded framebuffer as before
	static void framebufferBlit(const Framebuffer* a_From, const Framebuffer* a_To);

	//resizes this framebuffer and it's component to the new size
	void resizeFramebuffer(unsigned int a_Width, unsigned int a_Height);

	//does the final linkage for this Framebuffer
	//will add everything from addBuffer to the Framebuffer
	void genFramebuffer();

	//generates a Framebuffer with a Texture and a depth attachment
	void createRenderTarget();

	//adds a buffer of a_Type and a_Format to this framebuffer
	//will not create the framebuffer object
	//set the size of this framebuffer before using this class
	void addBuffer(FramebufferBufferTypes a_Type,
				   FramebufferBufferFormats a_Format,
				   unsigned int a_FormatSize);

	//adds a buffer of a_Type and a_Format to this framebuffer
	//size of the format will be decided in the cpp
	//will not create the framebuffer object
	//set the size of this framebuffer before using this class
	void addBuffer(FramebufferBufferTypes a_Type,
				   FramebufferBufferFormats a_Format);

	//returns a reference to the texture used by this Framebuffer
	//will be nullptr if no framebuffer has been generated
	Texture* getTexture(const unsigned int a_TextureIndex = 0) const;

	//returns the width of this framebuffer
	unsigned int getFramebufferWidth() const {
		return m_Width;
	}
	//returns the height of this framebuffer
	unsigned int getFramebufferHeight() const {
		return m_Height;
	}

	//returns the current openGL id for this Framebuffer
	//if texture hasent been bound, this will be 0
	const unsigned int getFramebufferId() {
		return m_Fbo;
	}
	
	//TODO: Move this out of framebuffer OR intergrate it better
	enum class GL_CALLS {
		DEPTH_TEST,
		CULL_FACE,
	};
	static unsigned int getGLCallFromEnum(GL_CALLS a_Call);
	static void glCall(GL_CALLS a_Call, bool a_Enabled);
private:
	//returns the unsigned int referencing OpenGL's format with it's size
	//will return the base version of the format if a incorrect format size is given
	unsigned int getGLFormatSize(FramebufferBufferFormats a_Format, unsigned int a_FormatSize);
	//returns the unsigned int referencing OpenGL's formats base
	//will return the base version of the format if a incorrect format size is given
	unsigned int getGLFormat(FramebufferBufferFormats a_Format);
	//returns the attachement GL value for a_Format
	//returns Attachment0 for rgba formats
	unsigned int getGLAttachment(FramebufferBufferFormats a_Format);

	unsigned int m_Width;
	unsigned int m_Height;

	//framebuffer object
	unsigned int m_Fbo;

	struct Component {
		//type of this component
		FramebufferBufferTypes m_Type;
		FramebufferBufferFormats m_Format;
		unsigned int m_FormatSize = 0;
	};

	struct FramebufferTexture : Component {
		//id for referencing the texture on opengl
		unsigned int m_TextureID = 0;
		Texture* m_TextureObject = nullptr;
		unsigned int m_BaseGLFormat = 0;
	};
	struct FramebufferRenderbuffer : Component {
		//id for referencing this component in opengl
		unsigned int m_RenderbufferID = 0;
	};

	//list of every attached component in this framebuffer
	std::vector<Component*> m_AttachedComponents;
	//list of all the color attachments in the framebuffer in order of their framebuffer
	std::vector<Component*> m_ColorAttachments;
	//list of attached components that are not color attachments
	std::vector<Component*> m_OtherAttachments;

	std::vector<Texture*> m_Textures;

};

