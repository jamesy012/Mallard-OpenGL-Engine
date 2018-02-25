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


void Text::generateText(const std::string a_Text, const int a_FontSize) {
	m_FontSizeRatio = a_FontSize == 0 ? 1.0f : (float)a_FontSize / m_LinkedFont->getFontSize();
	const float lineYOffset = m_LinkedFont->getLineOffset(1) * m_FontSizeRatio;

	m_Text = a_Text;
	//reset the text mesh
	if (m_TextMesh != nullptr) {
		//perhaps don't delete, but release the data instead
		delete m_TextMesh;
	}
	m_TextMesh = new Mesh();

	//data storage types from Mesh
	std::vector<MeshVerticesType> vertices;
	std::vector<MeshIndicesType> indexes;

	//resize the vectors so we have the correct ammount
	vertices.resize(a_Text.size() * 4);
	indexes.resize(a_Text.size() * 6);

	m_NumberOfLines = 1;

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
			m_NumberOfLines++;
			offsetY -= lineYOffset;
			offsetX = 0;
			continue;
		}
		//todo allow tab characters
		if (letter == '\t') {
			continue;
		}

		//get character info
		Font::GlyphData glyphInfo = m_LinkedFont->getGlyphInfo(letter, offsetX, offsetY);

		float xSize = glyphInfo.offsetX - offsetX;
		float ySize = glyphInfo.offsetY - offsetY;

		//set positions and uvs
		for (int q = 0; q < 4; q++) {
			// * (q%2==1? fontScale : 1)
			float posOffset = 0;//(q % 2 == 1 ? glyphInfo.positions[q].x - offsetX : 0);
			float posX = offsetX + (glyphInfo.positions[q].x - offsetX)* (q % 2 == 1 ? m_FontSizeRatio : 1)+ posOffset;
			float posY = offsetY + (glyphInfo.positions[q].y - -offsetY)* (q >=2 ? m_FontSizeRatio : 1) - lineYOffset;
			vertices[(i * 4) + q].position.x = posX;
			vertices[(i * 4) + q].position.y = posY;
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


		//update offset
		offsetX = offsetX + (xSize*m_FontSizeRatio);
		offsetY = offsetY + (ySize*m_FontSizeRatio);
	}

	//finally apply and bind that data
	m_TextMesh->applyData(vertices, indexes);

	//also tell the mesh which texture to use
	m_TextMesh->setTexture(m_LinkedFont->getFontTexture());

	m_TextMesh->bind();

	m_TextMesh->setDebugName("Text Mesh");
}

void Text::draw() {
	//just draw the Mesh if it's been made yet
	if (m_TextMesh != nullptr) {
		m_TextMesh->draw();
	}
}

void Text::drawInstance(unsigned int a_Amount) {
	_ASSERT_EXPR(false, L"Text has no Instanced Draw");
}

float Text::getLineOffset() const {
	return m_LinkedFont->getLineOffset(m_NumberOfLines) * m_FontSizeRatio;
}
