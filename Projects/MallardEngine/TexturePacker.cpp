#include "TexturePacker.h"

#include "Texture.h"

//packing based off 
//https://www.gamedev.net/forums/topic/683912-sprite-packing-algorithm-explained-with-example-code/
//code example https://jsfiddle.net/jLchftot/

TexturePacker::TexturePacker() {
	m_Width = DEFAULT_WIDTH;
	m_Height = DEFAULT_HEIGHT;
	m_PackedTexture = new Texture(m_Width, m_Height, TextureType::RGBA);

}

TexturePacker::TexturePacker(unsigned int a_Width, unsigned int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
	m_PackedTexture = new Texture(m_Width, m_Height, TextureType::RGBA);
}


TexturePacker::~TexturePacker() {
	delete m_PackedTexture;
	for (int i = 0; i < m_Boxes.size(); i++) {
		delete m_Boxes[i];
	}
}

bool TexturePacker::addTexture(const Texture * a_Texture) {
	TextureBox* tb = createTextureBox(a_Texture->m_TextureWidth, a_Texture->m_TextureHeight, a_Texture);
	if (tb == nullptr) {
		return false;
	}

	for (int y = 0; y < a_Texture->m_TextureWidth; y++) {
		for (int x = 0; x < a_Texture->m_TextureHeight; x++) {
			m_PackedTexture->setPixel(tb->x + x, tb->y + y, a_Texture->getPixel(x, y));
		}
	}

	return true;
}

bool TexturePacker::testAdd(const int a_Width, const int a_Height, const glm::vec4 a_Color) {
	TextureBox* tb = createTextureBox(a_Width, a_Height);
	if (tb == nullptr) {
		return false;
	}

	//test color add
	for (int y = 0; y < a_Height; y++) {
		for (int x = 0; x < a_Height; x++) {
			m_PackedTexture->setPixel(tb->x + x, tb->y + y, a_Color);
		}
	}

	return true;
}

void TexturePacker::bind() {
	m_PackedTexture->bindTexture();
}

bool TexturePacker::boxCollide(TextureBox * a_Box1, TextureBox* a_Box2) const {
	if (a_Box1->x					< a_Box2->x + a_Box2->width		&&
		a_Box1->x + a_Box1->width	> a_Box2->x						&&
		a_Box1->y					< a_Box2->y + a_Box2->height	&&
		a_Box1->y + a_Box1->height	> a_Box2->y						) {
		return true;
	}
	return false;
}

TexturePacker::TextureBox * TexturePacker::getBoxInsidePoint(const int a_X, const int a_Y) const {
	for (int i = 0; i < m_Boxes.size(); i++) {
		TextureBox* tb = m_Boxes[i];
		if (tb->x + tb->width >= a_X &&
			tb->x <= a_X &&
			tb->y + tb->height >= a_Y &&
			tb->y <= a_Y) {
			return tb;
		}
	}
	return nullptr;
}

TexturePacker::TextureBox * TexturePacker::getBoxInsideBox(TextureBox * a_Box) const {
	for (int i = 0; i < m_Boxes.size(); i++) {
		TextureBox* tb = m_Boxes[i];
		if (boxCollide(a_Box, tb)) {
			return tb;
		}
	}
	return nullptr;
}

TexturePacker::TextureBox * TexturePacker::createTextureBox(const int a_Width, const int a_Height, const Texture * a_Texture) {
	TextureBox* tb = new TextureBox{ 0,0,a_Width,a_Height, a_Texture };
	if (!findSpotForBox(tb)) {
		delete tb;
		tb = nullptr;
	}
	return tb;
}

bool TexturePacker::findSpotForBox(TextureBox * a_Box) {
	if (a_Box->width > m_Width || a_Box->height > m_Height) {
		printf("TexturePacker:: Texture is too big. Packer Size: (%i,%i) Image Size (%i,%i) Image: %s\n",
			   m_Width, m_Height,
			   a_Box->width, a_Box->height,
			   a_Box->texture == nullptr ? "Custom texture." : a_Box->texture->getFilename());
		return false;
	}
	if (m_Boxes.size() == 0) {
		m_Boxes.push_back(a_Box);
		return true;
	}
	{
		bool foundSpot = false;
		for (int y = 0; y < m_Height - a_Box->height; y += PACKING_Y_OFFSET) {
			a_Box->x = 0;
			a_Box->y = y;
			TextureBox* tb = m_Boxes[0];
			while (a_Box->x + a_Box->width <= m_Width) {
				while (tb != nullptr) {
					tb = getBoxInsideBox(a_Box);
					if (tb != nullptr) {
						a_Box->x += tb->width + 1;
					}

				}
				if (a_Box->x + a_Box->width > m_Width) {
					break;
				}
				if (tb == nullptr) {

					m_Boxes.push_back(a_Box);
					return true;
				}
			}
		}
	}

	printf("TexturePacker:: could not fit in texture, Please Increase the packer size. Image: (%i,%i) %s\n",
		   a_Box->width, a_Box->height,
		   a_Box->texture == nullptr ? "Custom texture." : a_Box->texture->getFilename());
	return false;
}
