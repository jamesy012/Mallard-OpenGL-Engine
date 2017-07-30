#include "Text.h"

#include "Font.h"
#include "Mesh.h"

Text::Text() {
}

Text::Text(const Font * a_Font) {
	//links up a_Font to be our font
	m_LinkedFont = a_Font;
}


Text::~Text() {
	if (m_TextMesh != nullptr) {
		delete m_TextMesh;
	}
}

void Text::linkFont(const Font * a_Font) {
	//if this is a different font from what we have loaded
	if (a_Font != m_LinkedFont) {
		m_LinkedFont = a_Font;
		//if we have set text before
		if (m_TextMesh != nullptr) {
			//generate the text again in the new font
			generateText(m_Text);
		}
	}
}


void Text::generateText(const std::string a_Text) {
	m_Text = a_Text;
	//reset the text mesh
	if (m_TextMesh != nullptr) {
		delete m_TextMesh;
	} else {
		m_TextMesh = new Mesh();
	}

	//data storage types from Mesh
	std::vector<MeshVerticesType> vertices;
	std::vector<MeshIndicesType> indexes;

	//resize the vectors so we have the correct ammount
	vertices.resize(a_Text.size() * 4);
	indexes.resize(a_Text.size() * 6);

	//store character offsets
	float offsetX = 0, offsetY = 0;
	//loop through each character
	for (size_t i = 0; i < a_Text.size(); i++) {
		//get a unsigned int of the current letter
		//using unsigned to allow special characters
		//without this special characters will be negative numbers and wont render
		unsigned char letter = a_Text[i];
		//check if the letter is a new line
		if (letter == '\n') {
			offsetY += m_LinkedFont->getLineOffset(1);
			offsetX = 0;
			continue;
		}

		//get character info
		Font::GlyphData glyphInfo = m_LinkedFont->getGlyphInfo(letter, offsetX, offsetY);
		//add character size offsets
		offsetX = glyphInfo.offsetX;
		offsetY = glyphInfo.offsetY;
		
		//set positions and uvs
		for (int q = 0; q < 4; q++) {
			vertices[(i * 4) + q].position.x = glyphInfo.positions[q].x;
			vertices[(i * 4) + q].position.y = glyphInfo.positions[q].y;
			vertices[(i * 4) + q].position.z = glyphInfo.positions[q].z;
			vertices[(i * 4) + q].position.w = 1.0f;
			vertices[(i * 4) + q].texCoord.x = glyphInfo.uvs[q].u;
			vertices[(i * 4) + q].texCoord.y = glyphInfo.uvs[q].v;
		}
		//set indices
		//0,1,3  0,3,2
		indexes[(i * 6) + 0] = ((i * 4) + 0);
		indexes[(i * 6) + 1] = ((i * 4) + 1);
		indexes[(i * 6) + 2] = ((i * 4) + 3);
		indexes[(i * 6) + 3] = ((i * 4) + 0);
		indexes[(i * 6) + 4] = ((i * 4) + 3);
		indexes[(i * 6) + 5] = ((i * 4) + 2);
	}

	//finally apply and bind that data
	m_TextMesh->applyData(vertices, indexes);

	m_TextMesh->bind();

	//also tell the mesh which texture to use
	m_TextMesh->setTexture(m_LinkedFont->getFontTexture());
}

void Text::draw() {
	//just draw the Mesh if it's been made yet
	if (m_TextMesh != nullptr) {
		m_TextMesh->draw();
	}

}
