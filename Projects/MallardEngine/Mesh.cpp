#include "Mesh.h"

#include <GL\glew.h>
#include <assimp\scene.h>

Mesh::Mesh() {
	m_Vao = m_Vbo = m_Ebo = 0;
}

Mesh::~Mesh() {
}

void Mesh::createBox() {
	//data from a obj file made in blender
	glm::vec4 vertPos[] = {
		glm::vec4(1, -1, -1, 1),
		glm::vec4(1, -1,  1, 1),
		glm::vec4(-1, -1,  1, 1),
		glm::vec4(-1, -1, -1, 1),
		glm::vec4(1,  1, -1, 1),
		glm::vec4(1,  1,  1, 1),
		glm::vec4(-1,  1,  1, 1),
		glm::vec4(-1,  1, -1, 1),
	};
	glm::vec4 normals[]{
		glm::vec4(0, -1,  0, 0),
		glm::vec4(0,  1,  0, 0),
		glm::vec4(1, -0,  0, 0),
		glm::vec4(-0, -0,  1, 0),
		glm::vec4(-1, -0, -0, 0),
		glm::vec4(0,  0, -1, 0),
	};
	glm::vec2 texCoords[]{//y coords have been flipped
		glm::vec2(1,0),
		glm::vec2(0,0),
		glm::vec2(0,1),
		glm::vec2(1,1),
	};
	unsigned int indexSize = 108;//by mousing over data below
	unsigned int indexData[]{
		2 , 1 , 1,	 3 , 2 , 1,	 4 , 3 , 1,
		8 , 2 , 2,	 7 , 3 , 2,	 6 , 4 , 2,
		1 , 4 , 3,	 5 , 1 , 3,	 6 , 2 , 3,
		2 , 4 , 4,	 6 , 1 , 4,	 7 , 2 , 4,
		7 , 1 , 5,	 8 , 2 , 5,	 4 , 3 , 5,
		1 , 3 , 6,	 4 , 4 , 6,	 8 , 1 , 6,
		1 , 4 , 1,	 2 , 1 , 1,	 4 , 3 , 1,
		5 , 1 , 2,	 8 , 2 , 2,	 6 , 4 , 2,
		2 , 3 , 3,	 1 , 4 , 3,	 6 , 2 , 3,
		3 , 3 , 4,	 2 , 4 , 4,	 7 , 2 , 4,
		3 , 4 , 5,	 7 , 1 , 5,	 4 , 3 , 5,
		5 , 2 , 6,	 1 , 3 , 6,	 8 , 1 , 6,
	};
	m_Vertices.reserve(indexSize);
	m_Indices.reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		Vertex vert;
		vert.position = vertPos[indexData[i] - 1];
		vert.texCoord = texCoords[indexData[i + 1] - 1];
		vert.normal = normals[indexData[i + 2] - 1];

		m_Vertices.push_back(vert);
		m_Indices.push_back(i / 3);
	}
	bind();
}

void Mesh::draw() {
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::applyData(std::vector<VerticesType> a_Verts, std::vector<IndicesType> a_Indices) {
	m_Vertices = a_Verts;
	m_Indices = a_Indices;
}

void Mesh::loadFromMesh(aiMesh * a_Mesh) {
	if (a_Mesh->HasPositions() && a_Mesh->HasFaces()) {
		unsigned int verticesAmount = a_Mesh->mNumVertices;
		unsigned int indicesAmount = a_Mesh->mNumFaces;

		m_Vertices.resize(verticesAmount);
		m_Indices.reserve(indicesAmount * 3);// could be wrong size, should generally be * 3 

		//vertices
		for (unsigned int i = 0; i < verticesAmount; i++) {

			Vertex vert;
			vert.position.x = a_Mesh->mVertices[i].x;
			vert.position.y = a_Mesh->mVertices[i].y;
			vert.position.z = a_Mesh->mVertices[i].z;
			vert.position.w = 1.0f;
			vert.normal.x = a_Mesh->mNormals[i].x;
			vert.normal.y = a_Mesh->mNormals[i].y;
			vert.normal.z = a_Mesh->mNormals[i].z;
			vert.normal.w = 0.0f;//0 or 1, 0 will do for now
			if (a_Mesh->mTextureCoords[0]) {
				vert.texCoord.x = a_Mesh->mTextureCoords[0][i].x;
				vert.texCoord.y = a_Mesh->mTextureCoords[0][i].y;
			} else {
				vert.texCoord = glm::vec2(0);
			}

			m_Vertices[i] = vert;

		}

		//indices
		unsigned int currIndex = 0;
		for (unsigned int i = 0; i < indicesAmount; i++) {
			aiFace face = a_Mesh->mFaces[i];
			unsigned int facesAmount = face.mNumIndices;

			for (unsigned int q = 0; q < facesAmount; q++) {
				//m_Indices[currIndex + q] = face.mIndices[q];
				m_Indices.push_back(face.mIndices[q]);
			}
			currIndex += facesAmount;
		}

		bind();
	}


	//materials
	//todo
	//needs scene to get materials


}

void Mesh::bind() {
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);
	//glGenBuffers(2, &m_Vbo); // this should do the same as above?!

	//bind all future data to the vao
	glBindVertexArray(m_Vao);

	//bind data
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(VerticesType), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(IndicesType), &m_Indices[0], GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) 0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) offsetof(Vertex, normal));

	//texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}