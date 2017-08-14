#version 410

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D TexDiffuse1;

vec4 boxBlur() {
	vec2 texel = 4.0f / textureSize(TexDiffuse1, 0).xy;
	
	vec4 color;
	color += texture(TexDiffuse1, vTexCoord + vec2(-texel.x,  texel.y));
	color += texture(TexDiffuse1, vTexCoord + vec2(-texel.x,  0));
	color += texture(TexDiffuse1, vTexCoord + vec2(-texel.x, -texel.y));
	color += texture(TexDiffuse1, vTexCoord + vec2(0	   ,  texel.y));
    color += texture(TexDiffuse1, vTexCoord); //middle
	color += texture(TexDiffuse1, vTexCoord + vec2(0	   , -texel.y));
	color += texture(TexDiffuse1, vTexCoord + vec2( texel.x,  texel.y));
	color += texture(TexDiffuse1, vTexCoord + vec2( texel.x, 0));
	color += texture(TexDiffuse1, vTexCoord + vec2( texel.x, -texel.y));
	
	return color / 9;
}

void main() {
	fragColor = boxBlur();
}