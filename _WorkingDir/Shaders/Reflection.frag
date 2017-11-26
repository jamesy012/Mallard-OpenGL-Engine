#version 410

in vec3 vPosition;
in vec3 vPosition2;
out vec4 fragColor; 
 
uniform vec4 color = vec4(1,1,1,1); 
uniform sampler2D TexDiffuse1; 

uniform vec2 resolution = vec2(1);
uniform float time = 0;
 
void main() { 
    vec2 screenUV = gl_FragCoord.xy/resolution;
    screenUV.x += (sin(time + (screenUV.x*30)+(time + (screenUV.y*28))))*0.002f;
    screenUV.y += (sin((time+1029) + (screenUV.x*0.15f)+((time + 1948) + (screenUV.y*0.1))))*0.005f;
    
    screenUV.x = clamp(screenUV.x,0,0.99f);
    screenUV.y = clamp(screenUV.y,0,0.99f);
    
	vec4 col = texture(TexDiffuse1, screenUV); 
    
    float colScales = abs((sin(((time*10) + (screenUV.y*0.8) + (time+(screenUV.y*5)*screenUV.x*1))/20) * 0.2) * 0.25f)+ 0.1;
    
    float alpha = -(vPosition2.z*10);
    //col *= 1-alpha*3;
	fragColor = col * vec4(colScales, 2*colScales, 0.7f, 1); 
    fragColor.a = 1-alpha;
} 
