#include "Mesh.h"

#include <GL\glew.h>
#include <assimp\scene.h>

Mesh::Mesh() {
	m_Vao = m_Vbo = m_Ebo = 0;
}

Mesh::~Mesh() {
}

void Mesh::draw() {
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
			vert.normal.x = a_Mesh->mNormals[i].x;
			vert.normal.y = a_Mesh->mNormals[i].y;
			vert.normal.z = a_Mesh->mNormals[i].z;
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) 0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) offsetof(Vertex, normal));

	//texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VerticesType), (GLvoid*) offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}
