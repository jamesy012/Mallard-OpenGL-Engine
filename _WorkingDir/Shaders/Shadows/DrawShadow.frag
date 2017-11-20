#version 410

in vec4 vNormal;
in vec4 vShadowCoord;
in vec2 vTexCoord;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 color = vec4(1);

uniform sampler2D shadowMap;
uniform sampler2D TexDiffuse1;

uniform float bias = 0.0035f;

void main() {
    float d;
    d = max(0,dot(normalize(vNormal.xyz),lightDir))*2.5;
    //d = 2;
    if(vShadowCoord.z < 1){
        if(texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - bias) {
            d = 0.35f;
        }
    }
    //d = vShadowCoord.z - bias;
    //d= texture(shadowMap, vShadowCoord.xy).r;
    //fragColor = vec4(d,d,d,1);
    fragColor = texture(TexDiffuse1, vTexCoord) * color * d;
    
    //d = dot(normalize(vNormal.xyz),lightDir)*1.5;
    //d = vShadowCoord.z;
    //fragColor = vec4(d,d,d,1);
    //fragColor = vec4(vShadowCoord.xyz,1);
    //fragColor = vNormal;
}
