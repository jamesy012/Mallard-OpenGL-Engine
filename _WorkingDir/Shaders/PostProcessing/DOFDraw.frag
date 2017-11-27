#version 410
//for the gaussian blur
//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
in vec2 vTexCoord;

out vec4 fragColor;

//rgb texture
uniform sampler2D TexDiffuse1;
//depth texture generated from DOFGen
uniform sampler2D depthTexture;

uniform float BlurResolution = 100;
uniform float radius = .2;
uniform vec2 dir = vec2(0);

vec4 addSum(vec2 a_UV){ 

    float dofValue = texture2D(depthTexture, vTexCoord+a_UV).r;
    //if(dofValue < 0.95){
    //    return vec4(1,1,1,1);
    //}
    //dofValue = 1;
    vec2 newOffset =vTexCoord - clamp(vTexCoord + a_UV,vec2(0),vec2(1));
    
    vec2 newUV = clamp(vTexCoord + ((a_UV) * dofValue),vec2(0),vec2(0.999));
   
    //return texture2D(depthTexture, newUV);
    return texture2D(TexDiffuse1, newUV);
}

void main() {

    float dofValue = texture2D(depthTexture, vTexCoord).r;

    //this will be our RGBA sum
	vec4 sum = vec4(0.0);
	
	//the amount to blur, i.e. how far off center to sample from 
	//1.0 -> blur by one pixel
	//2.0 -> blur by two pixels, etc.
	float blur = radius/BlurResolution; 
    
	//the direction of our blur
	//(1.0, 0.0) -> x-axis blur
	//(0.0, 1.0) -> y-axis blur
	float hstep = dir.x * dofValue;
	float vstep = dir.y * dofValue;
    
	//apply blurring, using a 9-tap filter with predefined gaussian weights
    
	sum += addSum( vec2(- 4.0*blur*hstep, - 4.0*blur*vstep)) * 0.0162162162;
	sum += addSum( vec2(- 3.0*blur*hstep, - 3.0*blur*vstep)) * 0.0540540541;
	sum += addSum( vec2(- 2.0*blur*hstep, - 2.0*blur*vstep)) * 0.1216216216;
	sum += addSum( vec2(- 1.0*blur*hstep, - 1.0*blur*vstep)) * 0.1945945946;
                        
	sum += addSum( vec2(0,0)) * 0.2270270270;
                        
	sum += addSum( vec2(+ 1.0*blur*hstep, + 1.0*blur*vstep)) * 0.1945945946;
	sum += addSum( vec2(+ 2.0*blur*hstep, + 2.0*blur*vstep)) * 0.1216216216;
	sum += addSum( vec2(+ 3.0*blur*hstep, + 3.0*blur*vstep)) * 0.0540540541;
	sum += addSum( vec2(+ 4.0*blur*hstep, + 4.0*blur*vstep)) * 0.0162162162;
    
	//discard alpha for our simple demo, multiply by vertex color and return
	fragColor = vec4(sum.rgb, 1.0);
     
}
