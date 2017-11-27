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
    
    vec2 offset;
    offset.x += (sin((time*0.2) + (screenUV.x*30)+(time + (screenUV.y*8))))*0.0045f;
    offset.y += (sin((time+1029) + (screenUV.x*0.15f)+((time + 1948) + (screenUV.y*0.1))))*0.001f;
    
    screenUV.x = clamp(screenUV.x+offset.x,0,0.99f);
    screenUV.y = clamp(screenUV.y+offset.y,0,0.99f);
    
	vec4 col = texture(TexDiffuse1, screenUV); 
    
    float colScales = abs((sin(((time*0.25) + (screenUV.y*4) + (time+(screenUV.y*5)*screenUV.x*10))) * 0.2) * 0.25f)+ 0.1;
    //float colScales = abs((sin(((time*1) + (screenUV.y) + (time+(screenUV.y)*screenUV.x))) * 0.2) * 0.25f)+ 0.1;
    
    float alpha = -(vPosition2.z*9);
    //col *= 1-alpha*3;
	fragColor = col * vec4(colScales, 1.2*colScales, 0.7f, 1); 
    fragColor.a = 1-alpha;
    //float dotVal = dot(normalize(vNormal.xyz),vec3(0,1,0));
    //fragColor += vec4(dotVal,dotVal,dotVal,1);
} 
