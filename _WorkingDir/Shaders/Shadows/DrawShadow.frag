#version 410

in vec4 vNormal;
in vec4 vShadowCoord;
in vec4 vVertexColor;
in vec2 vTexCoord;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 color = vec4(1);

uniform sampler2D shadowMap;

uniform sampler2D TexDiffuse1;

uniform float bias = 0.0036f;

void main() {

	float normalLightDot = dot(lightDir,normalize(vNormal.xyz));

    float d;
    d = 1-max(0,normalLightDot)*2.5;
	
    if(vShadowCoord.z < 1){
        if(texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - bias) {
            d = 0.35f;
        }
    }

	
    fragColor = texture(TexDiffuse1, vTexCoord) * color * vVertexColor * d;
	
}
