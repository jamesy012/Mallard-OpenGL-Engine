#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texCoods;
layout(location=3) in mat4 layoutModel;

out vec4 vNormal;
out vec4 vShadowCoord[3];
out vec2 vTexCoord;
out float EyeVertexZ;

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);
uniform mat4 normalRot = mat4(1);

uniform mat4 lightMatrix[3];

//change from float
uniform float INSTANCED = 0;

void main() {
    mat4 useableModel;
    if(INSTANCED >= 0.5){
        useableModel = layoutModel;
    }else{
        useableModel = model;
    }

    gl_Position = projectionViewMatrix * useableModel * position;
    vNormal = normalRot * normal;    
    vTexCoord = texCoods;
    
    //divided by the size of the shadow maps, this seems spot on
	EyeVertexZ = gl_Position.z/40;
	
	vShadowCoord[0] = lightMatrix[0] * useableModel * position;
	vShadowCoord[1] = lightMatrix[1] * useableModel * position;
	vShadowCoord[2] = lightMatrix[2] * useableModel * position;
}
