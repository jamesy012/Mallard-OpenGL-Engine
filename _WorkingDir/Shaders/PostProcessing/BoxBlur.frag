#version 410

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D TexDiffuse1;

uniform float Intensity = 4.0f;

vec4 boxBlur() {
	vec2 texel = Intensity / textureSize(TexDiffuse1, 0).xy;
	
    vec2 coord = clamp(vTexCoord, texel, vec2(1,1) - (texel * 2));
    
	vec4 color;
	color += texture(TexDiffuse1, coord + vec2(-texel.x,  texel.y));
	color += texture(TexDiffuse1, coord + vec2(-texel.x,  0));
	color += texture(TexDiffuse1, coord + vec2(-texel.x, -texel.y));
	color += texture(TexDiffuse1, coord + vec2(0	   ,  texel.y));
    color += texture(TexDiffuse1, coord); //middle
	color += texture(TexDiffuse1, coord + vec2(0	   , -texel.y));
	color += texture(TexDiffuse1, coord + vec2( texel.x,  texel.y));
	color += texture(TexDiffuse1, coord + vec2( texel.x, 0));
	color += texture(TexDiffuse1, coord + vec2( texel.x, -texel.y));
	
	return color / 9;
}

void main() {
    fragColor = boxBlur();
}
