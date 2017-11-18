#version 410

in vec4 vNormal;
in vec4 vShadowCoord;
in vec2 vTexCoord;

out vec4 fragColor;

uniform vec3 lightDir;

uniform sampler2D shadowMap;
uniform sampler2D TexDiffuse1;

uniform float bias = 0.0055f;

void main() {
    float d;
    d = max(0,dot(normalize(vNormal.xyz),lightDir))*2.5;
    //d = 1;
    if(vShadowCoord.z < 1){
        if(texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - bias) {
            d = 0.25f;
        }
    }
    //d = vShadowCoord.z - bias;
    //d= texture(shadowMap, vShadowCoord.xy).r;
    //fragColor = vec4(d,d,d,1);
    fragColor = texture(TexDiffuse1, vTexCoord) * d;
    
    //d = dot(normalize(vNormal.xyz),lightDir)*1.5;
    //fragColor = vec4(d,d,d,1);
    //fragColor = vec4(vShadowCoord.zzz,1);
}
