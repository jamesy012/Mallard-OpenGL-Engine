#version 410
 
in vec4 vVertColor;		
						
out vec4 fragColor; 
 
uniform vec4 color = vec4(1,1,1,1); 
 
void main() { 
	fragColor = vec4(1,0,1,0.5f); 
} 
