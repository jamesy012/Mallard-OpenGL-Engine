#pragma once

#include "DLLBuild.h"
#include "IRenderable.h"

#include <vector>
#include <string>



class Mesh;
class Texture;
struct FontCppData;

class DLL_BUILD Font : public IRenderable {
private:
	//struct GlyphData {
	//	struct {
	//		float x;
	//		float y;
	//		float z;
	//	} positions[4];
	//	struct {
	//		float u;
	//		float v;
	//	} uvs[4];
	//	float offsetX;
	//	float offsetY;
	//};
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
	struct vec2 {
		float u;
		float v;
		vec2() {};
		vec2(float a_U, float a_V) {
			u = a_U;
			v = a_V;
		};
	};
	struct GlyphData {
		struct vec3 positions[4];
		struct vec2 uvs[4];
		float offsetX;
		float offsetY;
	};
public:
	Font();
	~Font();

	void loadFont(const char* a_FontPath, float a_FontSize);

	void genText(std::string a_Text);

	// Inherited via IRenderable
	virtual void draw() override;

private:
	GlyphData getGlyphInfo(int a_Character, float a_OffsetX, float a_OffsetY);
	std::vector<unsigned char> loadFile(const char* a_FilePath);

	//to prevent font.h from having the stb_truetype included in it, this struct is in the cpp
	FontCppData* m_Data;

	Mesh* m_TextMesh = nullptr;

	Texture* m_Texture = nullptr;
	unsigned int m_TextureId = 0;

	//size of the font that was loaded
	float m_FontSize;
};

