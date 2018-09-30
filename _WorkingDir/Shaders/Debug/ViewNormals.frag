#version 410

out vec4 fragColor; 
 
uniform vec4 color = vec4(1,1,0,1); 
 
void main() { 
	fragColor = color; 
} 
