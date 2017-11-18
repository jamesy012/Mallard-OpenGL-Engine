#version 410

in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 fragColor;

uniform vec3 lightDir;

uniform sampler2D shadowMap;
uniform float bias = 0.0055f;

void main() {
	float d = max(0,dot(normalize(vNormal.xyz),lightDir));
    if(texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - bias) {
		d = 0.0f;
	}
    
    fragColor = vec4(d,d,d,1);
    
}
