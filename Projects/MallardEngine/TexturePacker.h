#pragma once
#include "DLLBuild.h"

#include <glm\glm.hpp>

#include <vector>

class Texture;

class DLL_BUILD TexturePacker {
public:
	struct TextureBox {
		unsigned int x, y;
		const unsigned int width, height;
		const Texture* texture;
	};

	TexturePacker();
	TexturePacker(unsigned int a_Width, unsigned int a_Height);
	TexturePacker(const TexturePacker& a_TexturePacker);
	~TexturePacker();

	TextureBox* addTexture(const Texture* a_Texture);
	TextureBox* testAdd(const unsigned int a_Width, const unsigned int a_Height, const glm::vec4 a_Color);
	void bind();

	unsigned int getWidth() const {
		return m_Width;
	}
	unsigned int getHeight() const {
		return m_Height;
	}

	Texture* m_PackedTexture;
private:
	//checks if two boxes collide
	bool boxCollide(TextureBox* a_Box1, TextureBox* a_Box2) const;
	TextureBox* getBoxInsidePoint(const unsigned int a_X, const unsigned int a_Y) const;
	TextureBox* getBoxInsideBox(TextureBox* a_Box) const;

	TextureBox* createTextureBox(const unsigned int a_Width, const unsigned int a_Height, const Texture* a_Texture = nullptr);

	bool findSpotForBox(TextureBox* a_Box);

	const unsigned int DEFAULT_WIDTH = 4096, DEFAULT_HEIGHT = 4096;
	const unsigned int PACKING_Y_OFFSET = 4;

	unsigned int m_Width, m_Height;

	std::vector<TextureBox*> m_Boxes;

};

