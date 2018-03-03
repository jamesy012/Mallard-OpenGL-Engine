#include "Text.h"

#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>

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


void Text::generateText(const std::string a_Text, const int a_FontSize, const glm::vec4 a_Color) {
	m_FontSizeRatio = a_FontSize == 0 ? 1.0f : (float)a_FontSize / m_LinkedFont->getFontSize();
	m_MaxFontSizeRatio = m_FontSizeRatio;
	float lineYOffset = m_LinkedFont->getLineOffset(1) * m_FontSizeRatio;

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

	glm::vec4 color = a_Color;

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

		if (letter == '[') {
			//closing ] of the code
			const size_t closeBBCode = a_Text.find_first_of(']', i);
			//starting point of the data
			const size_t startData = a_Text.find_first_of('=', i);

			//if any of those were npos, the code is broken so we skip it
			if (closeBBCode != a_Text.npos && startData != a_Text.npos) {

				//find the last index of the name (so we can have spaces)
				const size_t nameLen = a_Text.find_last_not_of(' ', startData-1);
				//get the name
				const std::string name = a_Text.substr(i + 1, nameLen - i);
				//get the data
				const std::string data = a_Text.substr(startData + 1, closeBBCode - startData - 1);

				//if we have [,] or = in our data, we are missing a ], so we skip
				if (data.find_first_of({ '[', ']', '=' }, 0) == data.npos) {

					//what kind of code are we looking at
					int bbCode = -1;
					//compare to list
					//todo: Convert to hash's so it's quicker??
					for (int q = 0; q < NUM_OF_BBCODES; q++) {
						if (name.compare(m_BBCodes[q].m_Name) == 0) {
							bbCode = q;
							break;
						}
					}

					//todo: prehaps swap the hardcoded case's with an enum thats with the m_BBCodes
					switch (bbCode) {
						case 0://size
						{
							const int fontSize = strtol(data.c_str(),NULL,10);
							//todo: allow empty data
							m_FontSizeRatio = (float)fontSize / m_LinkedFont->getFontSize();
							m_MaxFontSizeRatio = std::fmax(m_MaxFontSizeRatio, m_FontSizeRatio);
							lineYOffset = m_LinkedFont->getLineOffset(1) * m_FontSizeRatio;
						}
						break;
						case 1://color
						{
							float values[4] = { 1.0f,1.0f,1.0f,1.0f };
							size_t start = 0;
							size_t end = data.find_first_of(',');
							size_t index = 0;

							//if we have no data seperators then use the default color
							if (data.find_first_of(',') == data.npos) {
								index = 4;

								//probably not the best way, but it's quick
								memcpy(values, glm::value_ptr(a_Color), 4 * sizeof(float));
							}

							//go through the data, converting the 0-255 string to a 0-1 float
							while (start <= data.size() && index != 4) {
								std::string sub = data.substr(start, end - start);
								values[index++] = strtol(sub.c_str(),NULL,10) / 255.0f;
								start = end + 1;
								end = data.find_first_of(',', start);
								if (end == data.npos) {
									end = data.size();
								}
							}
							//if we have been given 3 or more values, then we are good
							if (index >= 3) {
								color = glm::make_vec4(values);
							} else {
								//Error - Not enough values supplied
								printf("BBCode - [Color] not enough variables supplied %s\n", data.c_str());

							}
						}
						break;
					}

					i = closeBBCode;
					continue;
				} else {
					//Error - Data had another BBcode within it
					printf("BBCode - [ detected, no = or ] found! Or space found within code - %s\n", data.c_str());

				}
			} else {
				//Error - BBCode had no End or data
				printf("BBCode - [ detected, no = or ] found!\n");
			}
		}

		//get character info
		Font::GlyphData glyphInfo = m_LinkedFont->getGlyphInfo(letter, offsetX, offsetY);

		float xSize = glyphInfo.offsetX - offsetX;
		float ySize = glyphInfo.offsetY - offsetY;

		//set positions and uvs
		for (int q = 0; q < 4; q++) {
			// * (q%2==1? fontScale : 1)
			float posOffset = 0;//(q % 2 == 1 ? glyphInfo.positions[q].x - offsetX : 0);
			float posX = offsetX + (glyphInfo.positions[q].x - offsetX)* (q % 2 == 1 ? m_FontSizeRatio : 1) + posOffset;
			float posY = offsetY + (glyphInfo.positions[q].y - -offsetY)* (q >= 2 ? m_FontSizeRatio : 1) - lineYOffset;
			vertices[(i * 4) + q].position.x = posX;
			vertices[(i * 4) + q].position.y = posY;
			vertices[(i * 4) + q].position.z = glyphInfo.positions[q].z;
			vertices[(i * 4) + q].position.w = 1.0f;
			vertices[(i * 4) + q].texCoord.x = glyphInfo.uvs[q].u;
			vertices[(i * 4) + q].texCoord.y = glyphInfo.uvs[q].v;

			vertices[(i * 4) + q].color = color;
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
	return m_LinkedFont->getLineOffset(m_NumberOfLines) * m_MaxFontSizeRatio;
}
