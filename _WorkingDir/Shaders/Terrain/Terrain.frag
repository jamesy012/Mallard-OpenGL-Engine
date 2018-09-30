#version 410

in vec4 vVertColor;
in vec3 geomColour;
						
out vec4 fragColor; 

 
uniform vec4 color = vec4(1,1,1,1); 
 
void main() { 
	fragColor = color * vec4(geomColour,1);// * vVertColor; 
} 
