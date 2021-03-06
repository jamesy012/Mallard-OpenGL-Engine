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
    offset.x += (sin((time*3) + (screenUV.x*30)+(time + (screenUV.y*8))))*0.0055f;
    offset.y += (sin((time+1029) + (screenUV.x*0.15f)+((time + 1948) + (screenUV.y*0.1))))*0.003f;
    
    screenUV.x = clamp(screenUV.x+offset.x,0,0.99f);
    screenUV.y = clamp(screenUV.y+offset.y,0,0.99f);
    
	vec4 col = texture(TexDiffuse1, screenUV); 
    
    float alpha = -((vPosition2.z*11));
    
    //change between different color tests
    //just comment out the line below
    //#define Color1
    
    #ifdef Color1
        float colScales = abs((sin(((time*0.25) + (screenUV.y*4) + (time+(screenUV.y*5)*screenUV.x*10))) * 0.2) * 0.25f)+ 0.1;
        fragColor = col * vec4(colScales, 1.2*colScales, 0.7f, 1);  
    #else 
        float colScales = abs(offset.x+0.25)*0.7;
        fragColor = col * vec4(colScales, 1.2*colScales, 0.9, 1) * (1-alpha*0.2); 
    #endif
       
    //float colScales = abs((sin(((time*1) + (screenUV.y) + (time+(screenUV.y)*screenUV.x))) * 0.2) * 0.25f)+ 0.1;
    
	
    fragColor.a = (1-alpha);
    //float dotVal = dot(normalize(vNormal.xyz),vec3(0,1,0));
    //fragColor += vec4(dotVal,dotVal,dotVal,1);
} 
