#include "Skybox.h"

#include <crtdbg.h>  

#include <GL\glew.h>
#include <stb_image.h>

#include "Camera.h"
#include "Shader.h"

Skybox::Skybox() {
	m_VAO = m_VBO = m_TextureID = 0;
}


Skybox::~Skybox() {
	delete m_SkyboxShader;
	glDeleteTextures(1, &m_TextureID);
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Skybox::genSkybox(const std::string a_Front, const std::string a_Back, const std::string a_Top, const std::string a_Bottom, const std::string a_Left, const std::string a_Right) {
	_ASSERT_EXPR(m_VAO == 0, L"Skybox already created");
	genMesh();

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	addSkyboxSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, a_Front);
	addSkyboxSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, a_Back);
	addSkyboxSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, a_Top);
	addSkyboxSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, a_Bottom);
	addSkyboxSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, a_Left);
	addSkyboxSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, a_Right);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	genShader();
}

void Skybox::genSkybox(const std::string a_Path) {
	genSkybox(a_Path + "negz.jpg",
			  a_Path + "posz.jpg",
			  a_Path + "posy.jpg",
			  a_Path + "negy.jpg",
			  a_Path + "negx.jpg",
			  a_Path + "posx.jpg");
}

void Skybox::assignCamera(Camera * a_AssignedCamera) {
	m_Camera = a_AssignedCamera;
}

void Skybox::draw() {
	if (m_Camera->m_Transform.getLastTransformUpdate() != m_LastCameraUpdate || m_Camera->m_Transform.isParentDirty() || m_LastCameraUpdate == -1 || m_Camera->isDirty()) {
		Transform cameraTransform = m_Camera->m_Transform;
		cameraTransform.setPosition(glm::vec3(0,0,0));
		glm::mat4 viewMatrix = glm::inverse(cameraTransform.getGlobalMatrix());
		m_CameraPV = m_Camera->getProjectionMatrix() * viewMatrix;
		m_LastCameraUpdate = m_Camera->m_Transform.getLastTransformUpdate();

		cameraTransform.setRotation(glm::quat());
		cameraTransform.setScale(50);
		m_CameraPV *= cameraTransform.getLocalMatrix();

	}

	glDisable(GL_DEPTH_TEST);

	Shader::use(m_SkyboxShader);

	ShaderUniformData* shaderPV = m_SkyboxShader->m_CommonUniforms.m_ProjectionViewMatrix;
	shaderPV->setData(&m_CameraPV);
	Shader::applyUniform(shaderPV);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glEnable(GL_DEPTH_TEST);

}

void Skybox::drawInstance(unsigned int a_Amount) {
	_ASSERT_EXPR(false, L"Cant Render Skybox with Instance");
}

void Skybox::genMesh() {
	float points[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::genShader() {
	const char* vertex = R"(
		#version 410
		layout(location=0) in vec3 position;
		out vec3 texCoords;
		uniform mat4 projectionViewMatrix;
		void main() {
			texCoords = position;
			gl_Position = projectionViewMatrix * vec4(position,1);
		}
		)";
	const char* fragment = R"(
		#version 410
		in vec3 texCoords;
		uniform samplerCube cubeTexture;
		out vec4 fragColor;
		void main() {
			 fragColor = texture(cubeTexture, texCoords);
		}
		)";
	m_SkyboxShader = new Shader();
	m_SkyboxShader->setFromText(ShaderTypes::TYPE_VERTEX, vertex);
	m_SkyboxShader->setFromText(ShaderTypes::TYPE_FRAGMENT, fragment);
	m_SkyboxShader->linkShader();
}

void Skybox::addSkyboxSide(unsigned int a_SideTarget, const std::string a_FilePath) {
	int textureWidth, textureHeight, imageFormat;
	unsigned char* textureData = stbi_load(a_FilePath.c_str(),
							  &textureWidth, &textureHeight, &imageFormat, STBI_default);

	if (textureData == nullptr) {
		printf("Could not load Skybox Image: %s\n", a_FilePath.c_str());
		return;
	}
	int textureGlType = 0;
	switch (imageFormat) {
		case STBI_rgb_alpha:
			textureGlType = GL_RGBA;
			break;
		case STBI_rgb:
			textureGlType = GL_RGB;
			break;
		default:
			printf("Could not load Skybox Image - invalid Format: %s\n", a_FilePath.c_str());
			stbi_image_free(textureData);
			return;
	}

	glTexImage2D(
		a_SideTarget,
		0,
		textureGlType,
		textureWidth,
		textureHeight,
		0,
		textureGlType,
		GL_UNSIGNED_BYTE,
		textureData);

	stbi_image_free(textureData);
}
