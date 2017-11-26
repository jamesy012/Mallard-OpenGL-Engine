#version 410
 
in vec2 vPosition;
in vec2 vPosition2;
out vec4 fragColor; 
 
uniform vec4 color = vec4(1,1,1,1); 
uniform sampler2D TexDiffuse1; 

uniform vec2 resolution = vec2(1);
uniform float time = 0;
 
void main() { 
    vec2 screenUV = gl_FragCoord.xy/resolution;

    screenUV.x += (sin(time + (vPosition2.x*0.05f)+(time + (vPosition2.y*0.1))))*0.005f;
    screenUV.y += (sin((time+1029) + (vPosition2.x*0.15f)+((time + 1948) + (vPosition2.y*0.1))))*0.005f;
    
    screenUV.x = clamp(screenUV.x,0,0.99f);
    screenUV.y = clamp(screenUV.y,0,0.99f);
    
	vec4 col = texture(TexDiffuse1, screenUV); 
    
    float colScales = abs((sin(((time*10) + (vPosition2.y*0.8) + (time+(screenUV.y*5)*vPosition2.x*1))/20) * 0.2) * 0.25f)+ 0.1;
    
	fragColor = col * vec4(colScales, colScales, 0.7f, 1); 
} 
