#pragma once
#include "DLLBuild.h"

class Framebuffer;
class Shader;
class ShaderUniformData;

class Texture;

#include <map>
#include <string>

class Mesh;

class DLL_BUILD DepthOfField {
public:
	DepthOfField();
	~DepthOfField();

	void create();

	//runs the depth of field code.
	//diffuse on texture 0
	//depth on texture 1
	void use(Framebuffer* a_ReadBuffers);

	void setValue(const std::string a_UniformName, float a_NewValue);

	unsigned int getDOFTexture();
	Framebuffer* getDOFFramebuffer();

	float m_ScaledSize = 3;
private:
	//Depth of field
	Framebuffer * m_DOF = nullptr;
	Framebuffer* m_DOFScaled = nullptr;
	Framebuffer* m_DOFScaledBlurred = nullptr;

	Shader* m_DOFShader = nullptr;
	Shader* m_BlurShader;

	Mesh* m_FullScreenQuad;

	std::map<const std::string, ShaderUniformData*> m_ValueMap;
	static const unsigned int m_ValueCount = 3;
	const std::string m_ValueNames[m_ValueCount] = { "focusDistance", "falloff", "minStrength" };
};

