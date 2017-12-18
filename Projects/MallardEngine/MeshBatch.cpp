#include "MeshBatch.h"

#include "Mesh.h"

MeshBatch::MeshBatch() {
	m_CombinedMesh = new Mesh();
}

MeshBatch::MeshBatch(MeshBatch& a_Copy) {
	m_CombinedMesh = new Mesh(*a_Copy.m_CombinedMesh);
}


MeshBatch::~MeshBatch() {
	delete m_CombinedMesh;
}

void MeshBatch::setFromBatch(MeshBatch * a_OtherBatch) {
	delete m_CombinedMesh;
	m_CombinedMesh = new Mesh(*a_OtherBatch->m_CombinedMesh);
}

void MeshBatch::bind() {
	m_CombinedMesh->bind();
}

void MeshBatch::add(Mesh * a_Mesh, glm::mat4 a_ModelMatrix) {
	const unsigned int vertArraySize = m_CombinedMesh->m_Vertices.size();
	const unsigned int indexArraySize = m_CombinedMesh->m_Indices.size();
	const unsigned int addVertArraySize = a_Mesh->m_Vertices.size();
	const unsigned int addIndexArraySize = a_Mesh->m_Indices.size();
	m_CombinedMesh->m_Vertices.resize(m_CombinedMesh->m_Vertices.size() + a_Mesh->m_Vertices.size());
	m_CombinedMesh->m_Indices.resize(m_CombinedMesh->m_Indices.size() + a_Mesh->m_Indices.size());

	int combinedVertIndex = vertArraySize;
	for (int i = 0; i < addVertArraySize; i++) {
		MeshVerticesType mv = a_Mesh->m_Vertices[i];
		mv.position = a_ModelMatrix * mv.position;
		m_CombinedMesh->m_Vertices[combinedVertIndex + i] = mv;
	}

	//this could be a memcpy
	int combinedIndexIndex = indexArraySize;
	for (int i = 0; i < addIndexArraySize; i++) {
		m_CombinedMesh->m_Indices[combinedIndexIndex + i] = combinedIndexIndex + a_Mesh->m_Indices[i];
	}

	if (m_CombinedMesh->m_CreatedTexture || m_CombinedMesh->m_Texture == nullptr) {
		m_CombinedMesh->setTexture(a_Mesh->m_Texture);
	}

	if (m_BindAfterAdd) {
		bind();
	}
}

void MeshBatch::draw() {
	m_CombinedMesh->draw();
}

void MeshBatch::drawInstance(unsigned int a_Amount) {
	m_CombinedMesh->drawInstance(a_Amount);
}
