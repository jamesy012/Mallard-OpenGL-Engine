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
	void generateText(const std::string a_Text);

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	//returns the string being used
	std::string getText() {
		return m_Text;
	}

private:
	//copy of the text that was loaded into this object
	std::string m_Text;

	//mesh to perpare text in and to render the text
	Mesh* m_TextMesh = nullptr;

	//the font which is linked to this text object
	const Font* m_LinkedFont = nullptr;

};

