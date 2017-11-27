#version 410
//for the gaussian blur
//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D TexDiffuse1;
uniform float BlurResolution = 100;
uniform float radius = .2;
uniform vec2 dir = vec2(0);

vec4 addSum(vec2 a_UV){
    a_UV = clamp(a_UV,vec2(0),vec2(1));
    return texture2D(TexDiffuse1, a_UV);
}

void main() {
    //this will be our RGBA sum
	vec4 sum = vec4(0.0);
	
	//our original texcoord for this fragment
	vec2 tc = vTexCoord;
	
	//the amount to blur, i.e. how far off center to sample from 
	//1.0 -> blur by one pixel
	//2.0 -> blur by two pixels, etc.
	float blur = radius/BlurResolution; 
    
	//the direction of our blur
	//(1.0, 0.0) -> x-axis blur
	//(0.0, 1.0) -> y-axis blur
	float hstep = dir.x;
	float vstep = dir.y;
    
	//apply blurring, using a 9-tap filter with predefined gaussian weights
    
	sum += addSum( vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
	sum += addSum( vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
	sum += addSum( vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
	sum += addSum( vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;
    
	sum += addSum( vec2(tc.x, tc.y)) * 0.2270270270;
    
	sum += addSum( vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
	sum += addSum( vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
	sum += addSum( vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
	sum += addSum( vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

	//discard alpha for our simple demo, multiply by vertex color and return
	fragColor = vec4(sum.rgb, 1.0);
}
