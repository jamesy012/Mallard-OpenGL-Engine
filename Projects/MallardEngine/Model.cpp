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
	if (m_Resource_IsMain) {
		for (size_t i = 0; i < m_Meshs.size(); i++) {
			delete m_Meshs[i];
		}
		for (size_t i = 0; i < m_ModelMeshData.size(); i++) {
			if (m_ModelMeshData[i]->m_Texture != nullptr && m_ModelMeshData[i]->m_Texture != Texture::m_White1x1Texture) {
				m_ModelMeshData[i]->m_Texture->unload();
			}
			delete m_ModelMeshData[i];
		}
	}
	m_Meshs.clear();
}

unsigned int Model::getResourceType() const {
	return ResourceTypes::RESOUCE_MODEL;
}

void Model::loadNode(aiNode * a_Node) {
	const unsigned int meshCount = a_Node->mNumMeshes;
	const unsigned int childCount = a_Node->mNumChildren;
	//const char* name = a_Node->mName.C_Str();

	aiMatrix4x4 transform = a_Node->mTransformation;

	aiQuaternion rotation;
	aiVector3D scale,position;

	transform.Decompose(scale, rotation, position);

	std::cout << a_Node->mName.C_Str() << std::endl;
	std::cout << "p: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
	std::cout << "r: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << ")\n";


	//go through mesh's
	for (size_t i = 0; i < meshCount; i++) {
		unsigned int meshIndex = a_Node->mMeshes[i];
		aiMesh* mesh = m_Scene->mMeshes[meshIndex];


		Mesh* myMesh = new Mesh();
		myMesh->loadFromMesh(mesh, m_Scene->mMaterials[mesh->mMaterialIndex]);
		m_Meshs[meshIndex] = myMesh;

		//apply texture to mesh
		myMesh->setTexture(m_ModelMeshData[mesh->mMaterialIndex]->m_Texture);
		myMesh->m_TextureIndex = mesh->mMaterialIndex;

		myMesh->bind();
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

	for (size_t i = 0; i < m_Scene->mNumMaterials; i++) {
		aiMaterial* mat = materials[i];
		aiString matPath;

		ModelMeshData* mmd = new ModelMeshData();

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
				mmd->m_Texture = tex;
			}
		} else {
			//no textures were found, lets just add a empty texture
			//since the assimp material index will still count this
			mmd->m_Texture = Texture::m_White1x1Texture;
		}

		//aiColor3D color(0.f, 0.f, 0.f);
		//m_Scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		//printf("%i, (%f,%f,%f) \n", i, color.r, color.g, color.b);
		
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

		m_ModelMeshData.push_back(mmd);
	}
}

IResource* Model::resourceCreate() {
	return new Model();
}

bool Model::resourceLoad() {
	Assimp::Importer importer;
	m_Scene = importer.ReadFile(m_Resource_FileName.c_str(), aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	
	if (m_Scene == nullptr || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || m_Scene->mRootNode == nullptr) {
		//model failed to load
		//unload();
		return false;
	}

	//load mesh's
	const unsigned int meshCount = m_Scene->mNumMeshes;

	m_Meshs.resize(meshCount);

	//material load
	loadTextures();

	loadNode(m_Scene->mRootNode);
	return true;
}

void Model::resourceCopy(IResource * a_Resource) {
	m_Resource_FileName = a_Resource->getFilename();
	Model* model = (Model*) a_Resource;
	//set up vector sizes
	m_Meshs.resize(model->m_Meshs.size());
	m_ModelMeshData.resize(model->m_ModelMeshData.size());

	//copy materials
	for (size_t i = 0; i < model->m_ModelMeshData.size(); i++) {
		m_ModelMeshData[i] = model->m_ModelMeshData[i];
		
	}

	//copy mesh
	for (size_t i = 0; i < model->m_Meshs.size(); i++) {
		//m_Meshs[i] = new Mesh(*model->m_Meshs[i]);
		//m_Meshs[i]->setTexture(m_Textures[model->m_Meshs[i]->m_TextureIndex]);
		m_Meshs[i] = model->m_Meshs[i];
	}

	//todo: write a real copy
	//resourceLoad();
}

void Model::resourceUnload() {

}

void Model::draw() {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		m_Meshs[i]->draw();
	}
}

void Model::drawInstance(unsigned int a_Amount) {
	for (size_t i = 0; i < m_Meshs.size(); i++) {
		m_Meshs[i]->drawInstance(a_Amount);
	}
}
