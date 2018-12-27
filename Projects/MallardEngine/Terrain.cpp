#include "Terrain.h"

#include "Application.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "Multithreading\MultithreadManager.h"
#include "Multithreading\MtmThread.h"
#include "Shader.h"
#include "Noise.h"

PerllinNoise pn;

Terrain::Terrain() {
	scale = 10;
}


Terrain::~Terrain() {
	for (size_t i = 0; i < m_Terrains.size(); i++) {
		delete m_Terrains[i]->m_LowLod;
		delete m_Terrains[i]->m_HighLod;
		delete m_Terrains[i];
	}
	m_HeightMap->unload();
	m_GroundTex->unload();
}

void Terrain::generate() {
	pn.generate(100);
	MultithreadManager::m_OpenGLThread->queueMethod(this, [](void* tp) {
		((Terrain*)tp)->m_Shader = new Shader();
		((Terrain*)tp)->m_Shader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/Terrain/Terrain.vert");
		((Terrain*)tp)->m_Shader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/Terrain/Terrain.frag");
		((Terrain*)tp)->m_Shader->setFromPath(ShaderTypes::TYPE_GEOMETRY, "Shaders/Terrain/Terrain.geom");
		((Terrain*)tp)->m_Shader->linkShader();
		ShaderUniformData* colorUniform = ((Terrain*)tp)->m_Shader->getUniform(ShaderUniformTypes::VEC4, "color");
		//colorUniform->setData(0, 1, 0, 1);

		((Terrain*)tp)->m_NormalTestShader = new Shader();
		((Terrain*)tp)->m_NormalTestShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/Debug/ViewNormals.vert");
		((Terrain*)tp)->m_NormalTestShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/Debug/ViewNormals.frag");
		((Terrain*)tp)->m_NormalTestShader->setFromPath(ShaderTypes::TYPE_GEOMETRY, "Shaders/Debug/ViewNormals.geom");
		((Terrain*)tp)->m_NormalTestShader->linkShader();

		((Terrain*)tp)->m_HeightMap = new Texture();
		((Terrain*)tp)->m_HeightMap->load("Textures/noise.png");
		((Terrain*)tp)->m_GroundTex = new Texture();
		((Terrain*)tp)->m_GroundTex->load("Models/test/leaves 5.png");

		float scale = ((Terrain*)tp)->scale * 2;
		float size = 5;
		int num = 512 / scale;
		num /= 2;
		num = 10;
		for (int x = -0; x < num; x++) {
			for (int y = -0; y < num; y++) {
				TerrainObj* part = new TerrainObj();
				part->m_Pos.setPosition(glm::vec3(x*size * 2, 0, y*size * 2));
				part->m_Pos.setScale(glm::vec3(size));
				//printf("Starting High lod, X: %f, Z: %f\n", x * scale, y * scale);
				//part->m_HighLod = ((Terrain*)tp)->createHighLodAtPoint(glm::vec3(x, 0, y) * scale);
				part->m_HighLod = ((Terrain*)tp)->createPlaneAtPoint(10,glm::vec3(x, 0, y) * scale);
				//printf("Starting Low lod\n");
				//part->m_LowLod = ((Terrain*)tp)->createLowLodAtPoint(glm::vec3(x, 0, y) * scale);
				part->m_LowLod = ((Terrain*)tp)->createPlaneAtPoint(3, glm::vec3(x, 0, y) * scale, -0.025f);
				((Terrain*)tp)->m_Terrains.push_back(part);
			}
		}
	});

}

void Terrain::draw() {
	//todo: remove direct mesh draw calls here

	Shader::use(m_Shader);
	Camera* camera = Application::m_Application->getCurrentCamera();
	m_Shader->m_CommonUniforms.m_ProjectionViewMatrix->setData(camera);
	glm::vec3 cameraForward = (glm::vec3(0, 0, -1) * camera->m_Transform.getGlobalRotation()) * glm::vec3(-1, 1, 1);
	for (size_t i = 0; i < m_Terrains.size(); i++) {
		//if(i == 0){
		glm::vec3 tileDir = glm::normalize((m_Terrains[i]->m_Pos.getGlobalPosition() - camera->m_Transform.getGlobalPosition()));
		float dot = glm::dot(cameraForward, tileDir);
		//printf("dot %f (%f,%f,%f) . (%f,%f,%f)\n", dot, cameraForward.x, cameraForward.y, cameraForward.z, tileDir.x, tileDir.y, tileDir.z);
		if (dot < 0) {
			continue;
			//}
		}


		m_Shader->m_CommonUniforms.m_ModelMatrix->setData(&m_Terrains[i]->m_Pos.getGlobalMatrix());
		float dist = glm::distance(camera->m_Transform.getGlobalPosition(), m_Terrains[i]->m_Pos.getGlobalPosition());
		if (dist > 50) {
			//m_Shader->m_CommonUniforms.m_Color->setData(1, 0, 0, 1);
			m_Shader->checkUniformChanges();
			m_Terrains[i]->m_LowLod->draw();
		} else {
			//m_Shader->m_CommonUniforms.m_Color->setData(0, 1, 0, 1);
			m_Shader->checkUniformChanges();
			m_Terrains[i]->m_HighLod->draw();
		}
	}

	Shader::use(m_NormalTestShader);
	m_NormalTestShader->m_CommonUniforms.m_ProjectionViewMatrix->setData(camera);
	for (size_t i = 0; i < m_Terrains.size(); i++) {
		//if(i == 0){
		glm::vec3 tileDir = glm::normalize((m_Terrains[i]->m_Pos.getGlobalPosition() - camera->m_Transform.getGlobalPosition()));
		float dot = glm::dot(cameraForward, tileDir);
		//printf("dot %f (%f,%f,%f) . (%f,%f,%f)\n", dot, cameraForward.x, cameraForward.y, cameraForward.z, tileDir.x, tileDir.y, tileDir.z);
		if (dot < 0) {
			continue;
			//}
		}

		m_NormalTestShader->m_CommonUniforms.m_ModelMatrix->setData(&m_Terrains[i]->m_Pos.getGlobalMatrix());
		float dist = glm::distance(camera->m_Transform.getGlobalPosition(), m_Terrains[i]->m_Pos.getGlobalPosition());
		if (dist > 50) {
			//m_Shader->m_CommonUniforms.m_Color->setData(1, 0, 0, 1);
			m_NormalTestShader->checkUniformChanges();
			m_Terrains[i]->m_LowLod->draw();
		} else {
			//m_Shader->m_CommonUniforms.m_Color->setData(0, 1, 0, 1);
			m_NormalTestShader->checkUniformChanges();
			m_Terrains[i]->m_HighLod->draw();
		}
	}

}

Mesh * Terrain::createHighLodAtPoint(glm::vec3 a_Pos) {
	Mesh* mesh = new Mesh();
	glm::vec4 vertPos[] = {
		glm::vec4(-1, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(-1, 0, 0, 1),
		glm::vec4(0, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(-1, 0, -1, 1),
		glm::vec4(0, 0, -1, 1),
		glm::vec4(1, 0, -1, 1),
	};
	for (int i = 0; i < sizeof(vertPos) / sizeof(glm::vec4); i++) {
		vertPos[i].y = getHeightFromMapAtWorldPos((glm::vec3(vertPos[i]) * scale) + a_Pos);
	}
	glm::vec4 normals[]{
		glm::vec4(0, 1, 0, 0),
	};
	glm::vec2 texCoords[]{//y coords have been flipped
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
	};
	unsigned int indexSize = 72;//by mousing over data below
	unsigned int indexData[]{
		1, 1, 1, 2, 2, 1, 4, 3, 1,
		2, 2, 1, 5, 4, 1, 4, 3, 1,
		2, 1, 1, 3, 2, 1, 5, 3, 1,
		3, 2, 1, 6, 4, 1, 5, 3, 1,

		4, 1, 1, 5, 2, 1, 7, 3, 1,
		5, 2, 1, 8, 4, 1, 7, 3, 1,
		5, 1, 1, 6, 2, 1, 8, 3, 1,
		6, 2, 1, 9, 4, 1, 8, 3, 1,
	};
	mesh->m_Vertices.reserve(indexSize);
	mesh->m_Indices.reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		MeshVertex vert;
		vert.position = vertPos[indexData[i] - 1];
		//glm::vec2 uv = texCoords[indexData[i + 1] - 1];
		glm::vec2 uv = glm::vec2((vert.position.x + 1) / 3, (vert.position.z + 1) / 3);

		vert.texCoord = uv;

		vert.normal = normals[indexData[i + 2] - 1];

		mesh->m_Vertices.push_back(vert);
		mesh->m_Indices.push_back(i / 3);
	}
	mesh->bind();
	mesh->setDebugName("Terrain - High");
	mesh->setTexture(m_GroundTex);
	return mesh;
}

Mesh * Terrain::createLowLodAtPoint(glm::vec3 a_Pos) {
	Mesh* mesh = new Mesh();
	glm::vec4 vertPos[] = {
		glm::vec4(-1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(-1, 0, -1, 1),
		glm::vec4(1, 0, -1, 1),
	};
	for (int i = 0; i < 4; i++) {
		vertPos[i].y = getHeightFromMapAtWorldPos(glm::vec3(vertPos[i] * scale) + a_Pos) - 0.05f;
	}
	glm::vec4 normals[]{
		glm::vec4(0, 1, 0, 0),
	};
	glm::vec2 texCoords[]{//y coords have been flipped
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
	};
	unsigned int indexSize = 18;//by mousing over data below
	unsigned int indexData[]{
		1, 1, 1, 2, 2, 1, 3, 3, 1,
		2, 2, 1, 4, 4, 1, 3, 3, 1,
	};
	printf("Size: low %f\n", vertPos[0].x);
	mesh->m_Vertices.reserve(indexSize);
	mesh->m_Indices.reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		MeshVertex vert;
		vert.position = vertPos[indexData[i] - 1];
		//glm::vec2 uv = texCoords[indexData[i + 1] - 1];
		glm::vec2 uv = glm::vec2((vert.position.x + 1) / 3, (vert.position.z + 1) / 3);

		vert.texCoord = uv;

		vert.normal = normals[indexData[i + 2] - 1];

		mesh->m_Vertices.push_back(vert);
		mesh->m_Indices.push_back(i / 3);
	}
	mesh->bind();
	mesh->setDebugName("Terrain - Low");
	mesh->setTexture(m_GroundTex);
	return mesh;
}

Mesh * Terrain::createPlaneAtPoint(unsigned int a_Size, glm::vec3 a_Pos, float a_YBias) {
	Mesh* mesh = new Mesh();

	std::vector<glm::vec4> vertPos;
	std::vector<unsigned int> indexData;

	for (int z = 0; z < a_Size; z++) {
	for (int x = 0; x < a_Size; x++) {
			glm::vec3 pos = glm::vec3(-(x / (a_Size - 1.0f) - 0.5f), 0, (z / (a_Size - 1.0f) - 0.5f)) * 2.0f;
			pos.y = getHeightFromMapAtWorldPos((pos * scale) + a_Pos) + a_YBias;
	
			vertPos.push_back(glm::vec4(pos,1));
		}
	}

	
	for (int z = 0; z < a_Size-1; z++) {
		for (int x = 0; x < a_Size-1; x++) {
			indexData.push_back((z*a_Size + x));
			indexData.push_back((z*a_Size + x) + 1);
			indexData.push_back((z*a_Size + x) + a_Size);

			indexData.push_back((z*a_Size + x) + 1);
			indexData.push_back((z*a_Size + x) + a_Size + 1);
			indexData.push_back((z*a_Size + x) + a_Size);
		}
	}

	mesh->m_Vertices.reserve(indexData.size());
	mesh->m_Indices.reserve(indexData.size());
	for (unsigned int i = 0; i < indexData.size(); i++) {
		MeshVertex vert;
		vert.position = vertPos[indexData[i]];
		//glm::vec2 uv = texCoords[indexData[i + 1] - 1];
		glm::vec2 uv = glm::vec2((vert.position.x * a_Size) / (a_Size), (vert.position.z * a_Size) / (a_Size));

		vert.texCoord = uv;

		//vert.normal = normals[indexData[i + 2] - 1];
		vert.normal = glm::vec4(0, 1, 0, 0);

		mesh->m_Vertices.push_back(vert);
		mesh->m_Indices.push_back(i);
	}
	mesh->bind();
	mesh->setDebugName("Terrain - point");
	mesh->setTexture(m_GroundTex);
	return mesh;
}

float Terrain::getHeightFromMapAtWorldPos(glm::vec3 a_WorldPos) {
	// m_HeightMap->getPixel(256+vertPos[i].x + a_Pos.x, 256 + vertPos[i].z + a_Pos.z).r;
	const int center = 512 / 2;
	int x = center + a_WorldPos.x;
	int y = center + a_WorldPos.z;
	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}
	if (x > 512) {
		x = 512;
	}
	if (y > 512) {
		y = 512;
	}
	//printf("x: %i y: %i\n", x, y);
	return  m_HeightMap->getPixel(x, y).r;
	//return  pn.noise(x, y,0.8f);
}
