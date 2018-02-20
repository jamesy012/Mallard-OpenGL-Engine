#include "Font.h"

#include <GL\glew.h>
#include <stdio.h>
#include <fstream>

#include <memory>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "Texture.h"
#include "Shader.h"
#include "Text.h"

#include "GLDebug.h"

struct {
	const unsigned int textureWidth = 1024;
	const unsigned int textureHeight = 1024;
} m_Font;

//holds the data from stb true type
//loaded from loadFont
//used in loading and getting character glyphs
struct FontCppData {
	std::unique_ptr<stbtt_packedchar[]> m_Info;
};

//go from space to then end of the special characters
const int firstChar = ' ';
const int charCount = 255 - ' ';

Font::Font() {
	m_Data = new FontCppData();
}
Font::~Font() {
	delete m_Data;
	//todo: delete opengl data
	if (m_Texture != nullptr) {
		delete m_Texture;
	}

}


void Font::loadFont(const char * a_FontPath, float a_FontSize) {
	if (m_Texture != nullptr) {
		return;
	}
	m_FontSize = a_FontSize;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

	m_Texture = new Texture(m_TextureId, m_Font.textureWidth, m_Font.textureHeight);

	GLDebug_NAMEOBJ(GL_TEXTURE, m_TextureId, (std::string("Font ") + a_FontPath).c_str());

}

//uses stbtt to get the character's quad position
//offset is added to the position via stbtt
//all data is then stored into the GlphaData struct
Font::GlyphData Font::getGlyphInfo(int a_Character, float a_OffsetX, float a_OffsetY) const {
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

float Font::drawText(const char * a_Text) const {
	//create text with a reference to this font
	Text text(this);
	//gen mesh using a_Text
	text.generateText(a_Text);
	//render it
	text.draw();

	return text.getLineOffset();
}

void Font::generateShaderCode(Shader * a_ShaderRef) {
	const char* textVertex = R"(
            #version 330 core
			layout(location = 0) in vec4 position;
			layout(location = 2) in vec2 texCoord0;

					uniform mat4 projectionViewMatrix;
			uniform mat4 model = mat4(1);

					        out vec2 uv0;
            void main()
	        {
	            gl_Position = projectionViewMatrix * model * position;
	            uv0 = texCoord0;
	        }
        )";
	const char* textFragment = R"(
	        #version 330 core
            uniform sampler2D TexDiffuse1;
			uniform vec4 color = vec4(1,1,1,1);
            in vec2 uv0;
	        out vec4 fragColor;
            void main()
	        {
                vec4 c = texture(TexDiffuse1, uv0);
    	        fragColor = c.rrrr * color;
	        }
	    )";

	a_ShaderRef->setFromText(ShaderTypes::TYPE_VERTEX, textVertex);
	a_ShaderRef->setFromText(ShaderTypes::TYPE_FRAGMENT, textFragment);
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