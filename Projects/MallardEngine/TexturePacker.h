#pragma once
#include "DLLBuild.h"

#include <glm\glm.hpp>

#include <vector>

class Texture;

class DLL_BUILD TexturePacker {
private:
	struct TextureBox {
		int x, y;
		const int width, height;
		const Texture* texture;
	};
public:
	TexturePacker();
	TexturePacker(unsigned int a_Width, unsigned int a_Height);
	~TexturePacker();

	bool addTexture(const Texture* a_Texture);
	bool testAdd(const int a_Width, const int a_Height, const glm::vec4 a_Color);
	void bind();

	Texture* m_PackedTexture;
private:
	//checks if two boxes collide
	bool boxCollide(TextureBox* a_Box1, TextureBox* a_Box2) const;
	TextureBox* getBoxInsidePoint(const int a_X, const int a_Y) const;
	TextureBox* getBoxInsideBox(TextureBox* a_Box) const;

	TextureBox* createTextureBox(const int a_Width, const int a_Height, const Texture* a_Texture = nullptr);

	bool findSpotForBox(TextureBox* a_Box);

	const unsigned int DEFAULT_WIDTH = 4096, DEFAULT_HEIGHT = 4096;
	const unsigned int PACKING_Y_OFFSET = 4;

	unsigned int m_Width, m_Height;

	std::vector<TextureBox*> m_Boxes;

};

