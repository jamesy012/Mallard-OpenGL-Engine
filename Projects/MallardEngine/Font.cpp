#include "Font.h"

#include <GL\glew.h>
#include <stdio.h>
#include <fstream>

#include <memory>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "Mesh.h"
#include "Texture.h"

struct {
	const unsigned int textureWidth = 1024;
	const unsigned int textureHeight = 1024;
} m_Font;

struct FontCppData {
	std::unique_ptr<stbtt_packedchar[]> m_Info;
};

const char firstChar = ' ';
const char charCount = '~' - ' ';

Font::Font() {
	m_Data = new FontCppData();
}
Font::~Font() {
	delete m_Data;
	//todo: delete opengl data
	if (m_Texture != nullptr) {
		delete m_Texture;
	}
	if (m_TextMesh != nullptr) {
		delete m_TextMesh;
	}
}


void Font::loadFont(const char * a_FontPath, float a_FontSize) {
	if (m_Texture != nullptr) {
		return;
	}
	auto fontData = loadFile(a_FontPath);

	//create the image data
	m_Data->m_Info = std::make_unique<stbtt_packedchar[]>(charCount);

	auto atlasData = std::make_unique<uint8_t[]>(m_Font.textureWidth * m_Font.textureHeight);

	stbtt_pack_context context;
	int result;

	//create the texture
	result = stbtt_PackBegin(&context, atlasData.get(), m_Font.textureWidth, m_Font.textureHeight, 0, 1, nullptr);
	if (!result) {
		printf("ERROR LOADING FONT _UNKNOWN ERROR_%i,%s\n", __LINE__, __FUNCTION__);
		return;//error
	}
	stbtt_PackSetOversampling(&context, 2, 2);


	//save text positions on the texture
	result = stbtt_PackFontRange(&context, fontData.data(), 0, a_FontSize, firstChar, charCount, m_Data->m_Info.get());
	if (!result) {
		printf("ERROR WITH FONT SIZE WHEN PACKING, not enough space in texture!\n");
		//continue anyway
		//return;//error
	}

	stbtt_PackEnd(&context);

	//convert into Texture class
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Font.textureWidth, m_Font.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

	m_Texture = new Texture(m_TextureId, m_Font.textureWidth, m_Font.textureHeight);
}

void Font::draw() {


	if (m_TextMesh != nullptr) {
		m_TextMesh->draw();
	}

}

void Font::genText(std::string  a_Text) {
	if (m_TextMesh != nullptr) {
		delete m_TextMesh;
	} else {
		m_TextMesh = new Mesh();
	}

	std::vector<MeshVerticesType> vertices;
	std::vector<MeshIndicesType> indexes;

	vertices.resize(a_Text.size() * 4);
	indexes.resize(a_Text.size() * 6);

	uint16_t lastIndex = 0;
	float offsetX = 0, offsetY = 0;
	for (size_t i = 0; i < a_Text.size(); i++) {
		GlyphData glyphInfo = getGlyphInfo(a_Text[i], offsetX, offsetY);
		offsetX = glyphInfo.offsetX;
		offsetY = glyphInfo.offsetY;

		for (int q = 0; q < 4; q++) {
			vertices[(i * 4) + q].position.x = glyphInfo.positions[q].x;
			vertices[(i * 4) + q].position.y = glyphInfo.positions[q].y;
			vertices[(i * 4) + q].position.z = glyphInfo.positions[q].z;
			vertices[(i * 4) + q].position.w = 1.0f;
			vertices[(i * 4) + q].texCoord.x = glyphInfo.uvs[q].u;
			vertices[(i * 4) + q].texCoord.y = glyphInfo.uvs[q].v;
		}

		indexes[(i * 6) + 0] = ((i * 4) + 0);
		indexes[(i * 6) + 1] = ((i * 4) + 1);
		indexes[(i * 6) + 2] = ((i * 4) + 3);
		indexes[(i * 6) + 3] = ((i * 4) + 0);
		indexes[(i * 6) + 4] = ((i * 4) + 3);
		indexes[(i * 6) + 5] = ((i * 4) + 2);
	}
	m_TextMesh->applyData(vertices, indexes);

	m_TextMesh->bind();

	m_TextMesh->setTexture(m_Texture);

}

Font::GlyphData Font::getGlyphInfo(char a_Character, float a_OffsetX, float a_OffsetY) {
	stbtt_aligned_quad quad;

	stbtt_GetPackedQuad(m_Data->m_Info.get(), m_Font.textureWidth, m_Font.textureHeight, a_Character - firstChar, &a_OffsetX, &a_OffsetY, &quad, 1);

	float xMin = quad.x0;
	float xMax = quad.x1;
	float yMin = -quad.y1;
	float yMax = -quad.y0;

	GlyphData gd;

	gd.offsetX = a_OffsetX;
	gd.offsetY = a_OffsetY;

	gd.positions[0] = { xMin, yMin, 0 };
	gd.positions[1] = { xMax, yMin, 0 };
	gd.positions[2] = { xMin, yMax, 0 };
	gd.positions[3] = { xMax, yMax, 0 };

	xMin = quad.s0;
	xMax = quad.s1;
	yMin = quad.t1;
	yMax = quad.t0;

	gd.uvs[0] = { xMin, yMin };
	gd.uvs[1] = { xMax, yMin };
	gd.uvs[2] = { xMin, yMax };
	gd.uvs[3] = { xMax, yMax };

	return gd;
}

std::vector<unsigned char> Font::loadFile(const char* a_FilePath) {
	std::ifstream file(a_FilePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		printf("Font failed to load, %s", a_FilePath);
		return std::vector<unsigned char>(0);
	}
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<unsigned char> bytes = std::vector<unsigned char>((unsigned int)size);
	file.read(reinterpret_cast<char*>(&bytes[0]), size);
	file.close();
	return bytes;
}