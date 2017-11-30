#version 410

in vec4 vNormal;
in vec4 vShadowCoord[3];
in vec2 vTexCoord;
in float EyeVertexZ;

out vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 color = vec4(1);

uniform sampler2D shadowMap[3];

uniform sampler2D TexDiffuse1;

uniform float bias = 0.0035f;

void main() {
	int farTest = int(2*clamp(EyeVertexZ,0,1));
	//farTest = 0;
    float d;
    d = max(0,dot(normalize(vNormal.xyz),lightDir))*2.5;
    //d = 2;
    if(vShadowCoord[farTest].z < 1){
        if(texture(shadowMap[farTest], vShadowCoord[farTest].xy).r < vShadowCoord[farTest].z - bias) {
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
    //fragColor = vec4(vShadowCoord[farTest].xyz,1);
    //fragColor = vNormal;
	//fragColor = texture(shadowMap[farTest], vShadowCoord[farTest].xy);
	//float value = gl_Position.z;
	//fragColor = vec4((gl_FragCoord.z/100)/gl_FragCoord.w);
	//fragColor = vec4(gl_FragCoord.w);
	//fragColor = vec4(EyeVertexZ);
	//
    //
    //if(gl_FragCoord.x > 900 && gl_FragCoord.x < 1000){
	//if(farTest == 0){
	//	fragColor = vec4(1,0,0,1);
	//}
	//if(farTest == 1){
	//	fragColor = vec4(0,1,0,1);
	//}
	//if(farTest == 2){
	//	fragColor = vec4(0,0,1,1);
	//}
	//if(farTest == 3){
	//	fragColor = vec4(1,0,1,1);
	//}
    //}
}
