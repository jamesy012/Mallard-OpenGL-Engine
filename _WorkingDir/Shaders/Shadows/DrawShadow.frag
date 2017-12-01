#version 410

//USE_RANDOM_OFFSETS uses parts from this tutorial http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
#define USE_RANDOM_OFFSETS

in vec4 vNormal;
in vec4 vShadowCoord;
in vec4 vVertexColor;
in vec2 vTexCoord;


out vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 color = vec4(1);

uniform sampler2D shadowMap;

uniform sampler2D TexDiffuse1;

uniform float bias = 0.003761f;
#ifdef USE_RANDOM_OFFSETS
in vec4 vWorldPosition;

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
#endif

void main() {

//common
	float normalLightDot = dot(lightDir,normalize(vNormal.xyz));
	float zz = texture(shadowMap, vShadowCoord.xy).r;
    float d;
	d = 1-max(0,normalLightDot)*2.5;
	
	//instead of using if(vShadowCoord.z < 1) {
	//we then mix the values later
    float shadowCoordIf = step(1.0f,vShadowCoord.z);
	
	//with random offsets
#ifdef USE_RANDOM_OFFSETS	
	if(vShadowCoord.z < 1){
		d = 1.0f;
		const int NUM_OF_LOOPS = 4;
		const float ratio = 1.0f/(NUM_OF_LOOPS+0.5f); 
		for (int i=0;i<NUM_OF_LOOPS;i++){
			// use either :
			//  - Always the same samples.
			//    Gives a fixed pattern in the shadow, but no noise
			//int index = i;
			//  - A random sample, based on the pixel's screen location. 
			//    No banding, but the shadow moves with the camera, which looks weird.
			//int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
			//  - A random sample, based on the pixel's position in world space.
			//    The position is rounded to the millimeter to avoid too much aliasing
			int index = int(16.0*random(floor(vWorldPosition.xyz*30), i))%16;
			
			vec2 pos = vShadowCoord.xy + poissonDisk[index]/(((zz*0.5+0.5)) * 450);
			//vec2 pos = vShadowCoord.xy + poissonDisk[index]/200.0f;
			
			if(texture(shadowMap, pos).r < vShadowCoord.z - bias) {
				d -= ratio;
			}
			
		}
	}
	
	//normal shadow check
#else
    if(zz < vShadowCoord.z - bias) {
        d = 0.35f;
    }	
	
	//d = texture(shadowMap, vShadowCoord.xy).r ;
	//d = smoothstep(texture(shadowMap, vShadowCoord.xy).r,0.8,1-(vShadowCoord.z-bias));
#endif

	d = mix(shadowCoordIf, 1, d);

	
    fragColor = texture(TexDiffuse1, vTexCoord) * color * vVertexColor * d;
	
}
