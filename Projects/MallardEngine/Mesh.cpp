#include "Mesh.h"

#include <GL\glew.h>
#include <assimp\scene.h>

#include "Texture.h"
#include "Shader.h"
#include "Logging.h"

Mesh::Mesh() {
	m_Vao = m_Vbo = m_Ebo = 0;

}

Mesh::~Mesh() {
	if (m_CreatedTexture) {
		delete m_Texture;
	}
	if (m_Vao != 0) {
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteBuffers(1, &m_Vbo);
		glDeleteBuffers(1, &m_Ebo);
	}
}

Mesh::Mesh(const Mesh & a_Mesh) {
	m_Vertices = a_Mesh.m_Vertices;
	m_Indices = a_Mesh.m_Indices;
	m_Texture = a_Mesh.m_Texture;
	m_CreatedTexture = false;
	if (a_Mesh.m_Texture != nullptr) {
		//maybe use the same texture??
		//m_Texture = new Texture();
		//m_Texture->load(a_Mesh.m_Texture->getFilename());
	}
	bind();
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
		MeshVertex vert;
		vert.position = vertPos[indexData[i] - 1];
		vert.texCoord = texCoords[indexData[i + 1] - 1];
		vert.normal = normals[indexData[i + 2] - 1];

		m_Vertices.push_back(vert);
		m_Indices.push_back(i / 3);
	}
	bind();
}

void Mesh::createPlane(bool a_FlipYUV) {
	glm::vec4 vertPos[] = {
		glm::vec4(-1,  0, 1, 1),
		glm::vec4(1,  0, 1, 1),
		glm::vec4(-1,  0,  -1, 1),
		glm::vec4(1,  0,  -1, 1),
	};
	glm::vec4 normals[]{
		glm::vec4(0, 1, 0, 0),
	};
	glm::vec2 texCoords[]{//y coords have been flipped
		glm::vec2(0,0),
		glm::vec2(1,0),
		glm::vec2(0,1),
		glm::vec2(1,1),
	};
	unsigned int indexSize = 18;//by mousing over data below
	unsigned int indexData[]{
		1 , 1 , 1,	 2 , 2 , 1,	 3 , 3 , 1,
		2 , 2 , 1,	 4 , 4 , 1,	 3 , 3 , 1,
	};
	m_Vertices.reserve(indexSize);
	m_Indices.reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		MeshVertex vert;
		vert.position = vertPos[indexData[i] - 1];
		glm::vec2 uv = texCoords[indexData[i + 1] - 1];
		if (a_FlipYUV) {
			vert.texCoord = glm::vec2(uv.x, 1 - uv.y);
		} else {
			vert.texCoord = uv;
		}
		vert.normal = normals[indexData[i + 2] - 1];

		m_Vertices.push_back(vert);
		m_Indices.push_back(i / 3);
	}
	bind();
}

void Mesh::draw() {
	unsigned int loc = 0;
	if (m_Texture != nullptr) {
		//todo move this out of mesh
		int slot = 0;
		Texture::bindAndApplyTexture(m_Texture, slot, Shader::getCurrentShader()->m_CommonUniforms.m_Textures[0]);
	}

	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	//unbind texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	Logging::objectRendered(m_Indices.size());
}

void Mesh::drawInstance(unsigned int a_Amount) {
	unsigned int loc = 0;
	if (m_Texture != nullptr) {
		//todo move this out of mesh
		int slot = 0;
		Texture::bindAndApplyTexture(m_Texture, slot, Shader::getCurrentShader()->m_CommonUniforms.m_Textures[0]);

	}

	glBindVertexArray(m_Vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, a_Amount);
	glBindVertexArray(0);

	//unbind texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	Logging::objectRendered(m_Indices.size() * a_Amount);
}

void Mesh::applyData(std::vector<MeshVerticesType> a_Verts, std::vector<MeshIndicesType> a_Indices) {
	m_Vertices = a_Verts;
	m_Indices = a_Indices;
}

void Mesh::loadFromMesh(aiMesh * a_Mesh, aiMaterial* a_Material) {
	if (a_Mesh->HasPositions() && a_Mesh->HasFaces()) {
		unsigned int verticesAmount = a_Mesh->mNumVertices;
		unsigned int indicesAmount = a_Mesh->mNumFaces;

		m_Vertices.resize(verticesAmount);
		m_Indices.reserve(indicesAmount * 3);// could be wrong size, should generally be * 3 

		//vertices
		for (unsigned int i = 0; i < verticesAmount; i++) {

			MeshVertex vert;
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

			aiColor4D color(0.0f, 0.0f, 0.0f,0.0f);
			a_Material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

			//if (a_Mesh->GetNumColorChannels() != 0) {
				vert.color.r = color.r;
				vert.color.g = color.g;
				vert.color.b = color.b;
				vert.color.a = color.a;
			//}

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

		//dont bind, the object that loads from aiScene/aiNode will not be rendered
		//we just need it for the data
		//bind();
	}
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
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(MeshVerticesType), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(MeshIndicesType), &m_Indices[0], GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*) 0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*) offsetof(MeshVertex, normal));

	//texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*) offsetof(MeshVertex, texCoord));

	//Vertex Colors
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*) offsetof(MeshVertex, color));


	glBindVertexArray(0);

	//simple fix to allow all meshs to have a basic texture
	//this could also create lots of texture objects on the gpu,
	//maybe create one 1x1 texture and keep using that instead
	if (m_Texture == nullptr) {
		m_Texture = new Texture();
		m_Texture->load1x1Texture();
		m_CreatedTexture = true;
	}
}

void Mesh::setTexture(Texture * a_Texture) {
	if (m_CreatedTexture) {
		delete m_Texture;
	}
	m_CreatedTexture = false;
	m_Texture = a_Texture;
}

Texture * Mesh::getTexture() const {
	return m_Texture;
}
