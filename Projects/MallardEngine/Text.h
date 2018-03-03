#pragma once
#include "DLLBuild.h"

#include "IRenderable.h"

#include <string>

class Font;
class Mesh;

class DLL_BUILD Text : public IRenderable {
public:
	Text();
	Text(const Font* a_Font);
	~Text();


	//links this text object up with a_Font
	//does not check if a_Font is legit or nullptr
	void linkFont(const Font* a_Font);

	//generates a mesh based off a_Text
	//will overwrite the old mesh/create a new one if there was none before
	//goes left to right
	//can handle new lines
	//will cause a error if a_Font has not been linked or is invalid
	//a_FontSize can be 0, if it's 0 then the default size from the linked font will be used
	//a_Color's value should between 0 and 1
	void generateText(const std::string a_Text, const int a_FontSize = 0, const glm::vec4 a_Color = glm::vec4(1));

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	//returns the string being used
	std::string getText() {
		return m_Text;
	}

	//returns number of new lines in this text
	//will be 0 if text has not been generated
	//will be 1 or above after generation
	int getNumberOfLines() const {
		return m_NumberOfLines;
	}

	float getLineOffset() const;

private:
	//copy of the text that was loaded into this object
	std::string m_Text;

	//mesh to perpare text in and to render the text
	Mesh* m_TextMesh = nullptr;

	//the font which is linked to this text object
	const Font* m_LinkedFont = nullptr;

	unsigned int m_NumberOfLines = 0;

	//the ratio compared to the linked font size
	float m_FontSizeRatio = 1.0f;
	float m_MaxFontSizeRatio = 1.0f;

	const static unsigned int NUM_OF_BBCODES = 2;
	const struct {
		const char* m_Name;
	} m_BBCodes[NUM_OF_BBCODES] = { {"Size"}, {"Color"} };
};

