#include "Model.h"

#include <iostream>

/* assimp include files. These three are usually needed. */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

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

		//apply texture to mesh
		myMesh->setTexture(m_Textures[mesh->mMaterialIndex]);
	}

	//go through children
	for (size_t i = 0; i < childCount; i++) {
		loadNode(a_Node->mChildren[i]);
	}
}

void Model::loadTextures() {
	//http://assimp.sourceforge.net/lib_html/materials.html
	aiMaterial** materials = m_Scene->mMaterials;

	aiReturn res;

	for (int i = 0; i < m_Scene->mNumMaterials; i++) {
		aiMaterial* mat = materials[i];
		aiString matPath;
		//todo add materials
		int numTex = mat->GetTextureCount(aiTextureType_DIFFUSE);
		if (numTex >= 1) {
			res = mat->GetTexture(aiTextureType_DIFFUSE, 0, &matPath);
			if (res == aiReturn_SUCCESS) {
				//get from start of string to the last / + 1 so it includes the /
				std::string path = m_Resource_FileName.substr(0, m_Resource_FileName.find_last_of('/') + 1);
				path += matPath.C_Str();//add material string

										//create texture and add to vector
				Texture* tex = new Texture();
				tex->load(path.c_str());
				m_Textures.push_back(tex);
			}
		} else {
			//no textures were found, lets just add a empty texture
			//since the assimp material index will still count this
			m_Textures.push_back(nullptr);
		}

		//testing getting material propertys
		//float test = 0;
		//char* zz;
		//aiMaterialProperty* prop;
		//res = aiGetMaterialProperty(mat, AI_MATKEY_TEXFLAGS_DIFFUSE(0), (const aiMaterialProperty**)&prop);
		//if (res == aiReturn_SUCCESS) {
		//	aiGetMaterialFloat(mat, AI_MATKEY_OPACITY(0), &test);
		//	mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), zz);
		//	aiColor4D test2;
		//	test2.a = 0;
		//
		//}
	}
}

void Model::resourceLoad() {
	Assimp::Importer importer;
	m_Scene = importer.ReadFile(m_Resource_FileName.c_str(), aiProcess_FlipUVs | aiProcess_GenNormals);
	
	if (m_Scene == nullptr || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || m_Scene->mRootNode == nullptr) {
		//model failed to load
		unload();
		return;
	}

	//load mesh's
	const unsigned int meshCount = m_Scene->mNumMeshes;

	m_Meshs.resize(meshCount);

	//material load
	loadTextures();

	loadNode(m_Scene->mRootNode);
}

void Model::resourceCopy(IResource * a_Resource) {
}

void Model::resourceUnload() {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		delete m_Meshs[i];
	}
	for (size_t i = 0; i < m_Textures.size(); i++) {
		if (m_Textures[i] != nullptr) {
			m_Textures[i]->unload();
		}
	}
	m_Meshs.clear();
}

void Model::draw() {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		m_Meshs[i]->draw();
	}
}
