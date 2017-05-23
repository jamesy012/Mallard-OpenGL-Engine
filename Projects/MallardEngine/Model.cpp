#include "Model.h"

#include <iostream>

/* assimp include files. These three are usually needed. */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

Model::Model() {
}


Model::~Model() {
}

unsigned int Model::getResourceType() const {
	return ResourceTypes::RESOUCE_MODEL;
}

void Model::loadNode(aiNode * a_Node) {
	const unsigned int meshCount = a_Node->mNumMeshes;
	const unsigned int childCount = a_Node->mNumChildren;
	//const char* name = a_Node->mName.C_Str();

	//go through mesh's
	for (size_t i = 0; i < meshCount; i++) {
		unsigned int meshIndex = a_Node->mMeshes[i];
		aiMesh* mesh = m_Scene->mMeshes[meshIndex];

		Mesh* myMesh = new Mesh();
		myMesh->loadFromMesh(mesh);
		m_Meshs[meshIndex] = myMesh;
	}

	for (size_t i = 0; i < childCount; i++) {
		loadNode(a_Node->mChildren[i]);
	}
}

void Model::resourceLoad() {
	Assimp::Importer importer;
	m_Scene = importer.ReadFile(m_FileName.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	
	if (m_Scene == nullptr || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || m_Scene->mRootNode == nullptr) {
		//model failed to load
		unload();
		return;
	}

	//load mesh's
	const unsigned int meshCount = m_Scene->mNumMeshes;

	m_Meshs.resize(meshCount);

	loadNode(m_Scene->mRootNode);
}

void Model::resourceCopy(IResource * a_Resource) {
}

void Model::resourceUnload() {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		delete m_Meshs[i];
	}
	m_Meshs.clear();
}

void Model::draw() {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		m_Meshs[i]->draw();
	}
}
