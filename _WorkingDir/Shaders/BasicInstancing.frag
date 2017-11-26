#version 410
 
in vec2 vTexCoord; 
in vec4 vVertColor;		
						
out vec4 fragColor; 
 
uniform vec4 color = vec4(1,1,1,1); 
uniform sampler2D TexDiffuse1; 
 
void main() { 
	vec4 col = texture(TexDiffuse1, vTexCoord); 
//	fragColor = (col * color)*col.a; 
//	fragColor = vec4(col.xyz,1); 
	fragColor = col * color * vVertColor; 
} 
