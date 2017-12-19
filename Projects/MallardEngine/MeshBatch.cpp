#include "MeshBatch.h"

#include "Mesh.h"
#include "Model.h"
#include "TexturePacker.h"

MeshBatch::MeshBatch() {
	m_CombinedMesh = new Mesh();
	m_TexturePacked = new TexturePacker();

	m_CombinedMesh->setTexture(m_TexturePacked->m_PackedTexture);
}

MeshBatch::MeshBatch(unsigned int a_TexturePackWidth, unsigned int a_TexturePackHeight) {
	m_CombinedMesh = new Mesh();
	m_TexturePacked = new TexturePacker(a_TexturePackWidth, a_TexturePackHeight);

	m_CombinedMesh->setTexture(m_TexturePacked->m_PackedTexture);
}

MeshBatch::MeshBatch(MeshBatch& a_Copy) {
	m_CombinedMesh = new Mesh(*a_Copy.m_CombinedMesh);
	m_TexturePacked = new TexturePacker(*a_Copy.m_TexturePacked);

	m_CombinedMesh->setTexture(m_TexturePacked->m_PackedTexture);
}


MeshBatch::~MeshBatch() {
	delete m_CombinedMesh;
	delete m_TexturePacked;
}

void MeshBatch::setFromBatch(MeshBatch * a_OtherBatch) {
	delete m_CombinedMesh;
	delete m_TexturePacked;

	m_CombinedMesh = new Mesh(*a_OtherBatch->m_CombinedMesh);
	m_TexturePacked = new TexturePacker(*a_OtherBatch->m_TexturePacked);

	m_CombinedMesh->setTexture(m_TexturePacked->m_PackedTexture);
}

void MeshBatch::bind() {
	m_CombinedMesh->bind();
	m_TexturePacked->bind();
}

void MeshBatch::add(Mesh * a_Mesh, glm::mat4 a_ModelMatrix) {
	TexturePacker::TextureBox* tb = m_TexturePacked->addTexture(a_Mesh->getTexture());	

	const unsigned int vertArraySize = m_CombinedMesh->m_Vertices.size();
	const unsigned int indexArraySize = m_CombinedMesh->m_Indices.size();
	const unsigned int addVertArraySize = a_Mesh->m_Vertices.size();
	const unsigned int addIndexArraySize = a_Mesh->m_Indices.size();
	m_CombinedMesh->m_Vertices.resize(m_CombinedMesh->m_Vertices.size() + a_Mesh->m_Vertices.size());
	m_CombinedMesh->m_Indices.resize(m_CombinedMesh->m_Indices.size() + a_Mesh->m_Indices.size());

	int combinedVertIndex = vertArraySize;
	for (int i = 0; i < addVertArraySize; i++) {
		MeshVerticesType mv = a_Mesh->m_Vertices[i];
		//position change
		mv.position = a_ModelMatrix * mv.position;

		//uv change
		mv.texCoord.x = (tb->x + (mv.texCoord.x * tb->width)) / m_TexturePacked->getWidth();
		mv.texCoord.y = (tb->y + (mv.texCoord.y * tb->height)) / m_TexturePacked->getHeight();

		m_CombinedMesh->m_Vertices[combinedVertIndex + i] = mv;
	}

	//this could be a memcpy
	int combinedIndexIndex = indexArraySize;
	for (int i = 0; i < addIndexArraySize; i++) {
		m_CombinedMesh->m_Indices[combinedIndexIndex + i] = combinedVertIndex + a_Mesh->m_Indices[i];
	}



	if (m_BindAfterAdd) {
		bind();
	}
}

void MeshBatch::add(Model * a_Model, glm::mat4 a_ModelMatrix) {
	//will have to change how this works after implenting a model matrix for each model
	//that will the case after adding in animation
	for (int i = 0; i < a_Model->m_Meshs.size(); i++) {
		add(a_Model->m_Meshs[i], a_ModelMatrix);
	}
}

void MeshBatch::draw() {
	m_CombinedMesh->draw();
}

void MeshBatch::drawInstance(unsigned int a_Amount) {
	m_CombinedMesh->drawInstance(a_Amount);
}
