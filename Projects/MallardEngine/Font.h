#pragma once

#include "DLLBuild.h"

#include <vector>

class Texture;
struct FontCppData;

class Shader;

class DLL_BUILD Font {
public:
	//simple storage for a vec3, used for quad char positions
	struct vec3 {
		float x;
		float y;
		float z;
		vec3() {};
		vec3(float a_X, float a_Y, float a_Z) {
			x = a_X;
			y = a_Y;
			z = a_Z;
		};
	};
	//simple storage for a vec2, used for uvs
	struct vec2 {
		float u;
		float v;
		vec2() {};
		vec2(float a_U, float a_V) {
			u = a_U;
			v = a_V;
		};
	};
	//storage for the glyph data
	//this holds the size plus the offset of a character
	//uv coords point towards the texture created by this font
	struct GlyphData {
		struct vec3 positions[4];
		struct vec2 uvs[4];
		float offsetX;
		float offsetY;
	};
public:
	Font();
	~Font();

	//loads the font with a font size
	//will print a error if there is something wrong
	//will not load font if it has already been loaded
	void loadFont(const char* a_FontPath, float a_FontSize);

	//gets the character size, uv coords and how far to offset it
	GlyphData getGlyphInfo(int a_Character, float a_OffsetX, float a_OffsetY) const;

	//simple and easy way to render some text
	//it uses whatever shader is currently in use
	//and creates a new text object to generate and render the text
	//returns the line offset * number of lines
	float drawText(const char* a_Text) const;

	//will apply a basic text rendering shader to the Vertex and Fragment Shaders
	//does not link the shader, or check if the shader has already been linked
	//does not check if a_ShaderRef is valid
	static void generateShaderCode(Shader* a_ShaderRef);

	//returns the font size of this Font
	//if no font has been loaded it will return 0
	float getFontSize() const {
		return m_FontSize;
	}

	//returns the size of a line of font multiplied by the number of lines
	float getLineOffset(int a_NumOfLines) const {
		return (m_FontSize * a_NumOfLines) * 0.8f;
	}

	//returns the font texture of this Font
	//if no font has been loaded it will return nullptr
	Texture* getFontTexture() const {
		return m_Texture;
	}

	//has this font been loaded
	bool hasLoaded() const {
		return m_Texture != nullptr;
	}
private:
	std::vector<unsigned char> loadFile(const char* a_FilePath);

	//to prevent font.h from having the stb_truetype included in it, this struct is in the cpp
	FontCppData* m_Data;

	Texture* m_Texture = nullptr;
	unsigned int m_TextureId = 0;

	//size of the font that was loaded
	float m_FontSize = 0;
};

