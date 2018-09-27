#pragma once
#include "DLLBuild.h"

#include <vector>

#include "Transform.h"

class Mesh;
class Shader;
class Texture;


struct TerrainObj {
	Transform m_Pos;
	Mesh* m_HighLod;
	Mesh* m_LowLod;
};

class DLL_BUILD Terrain {
public:
	Terrain();
	~Terrain();

	void generate();

	void draw();

	float scale = 2;
protected:
	Mesh * createHighLodAtPoint(glm::vec3 a_Pos);
	Mesh* createLowLodAtPoint(glm::vec3 a_Pos);
	Mesh* createPlaneAtPoint(unsigned int a_Size,glm::vec3 a_Pos);
	float getHeightFromMapAtWorldPos(glm::vec3 a_WorldPos);
private:
	Texture * m_HeightMap;
	Texture * m_GroundTex;
	Shader * m_Shader;
	std::vector<TerrainObj*> m_Terrains;
};

