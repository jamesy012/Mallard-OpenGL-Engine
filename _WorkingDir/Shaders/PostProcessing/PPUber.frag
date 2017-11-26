#version 410

//TODO: add preprocessor defines to this, to include what is needed

/** COMMON */
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D TexDiffuse1;

uniform vec2 resolution;

uniform float brightness = 1.0f;
uniform float contrast = -0.1f;
uniform float saturation = 1.2f;
uniform vec3 luminanceWeights = vec3(0.2126f,0.5152f,0.1722f);
uniform vec3 gamma = vec3(1.25f);

//radius of our vignette, where 0.5 results in a circle fitting the screen
uniform float vignetteRadius = 0.75f;
//softness of our vignette, between 0.0 and 1.0
uniform float vignetteSoftness = 0.3f;
//center of the vignette
uniform vec2 vignetteCenter = vec2(0.5f);

vec4 finalColor = vec4(1);

void vignette(){
	//determine center position
	vec2 position = (gl_FragCoord.xy / resolution.xy) - vignetteCenter;
	
	//determine the vector length of the center position
	float len = length(position);
	
	//use smoothstep to create a smooth vignette
	float vignetteAmount = smoothstep(vignetteRadius, vignetteRadius - vignetteSoftness, len);
	
	//apply the vignette with 50% opacity
	finalColor.rgb = mix(finalColor.rgb,finalColor.rgb * vignetteAmount,0.7f);
}
    
void main() {
    finalColor = texture(TexDiffuse1, vTexCoord);
    
    //saturation
    float luminance = dot(finalColor.rgb, luminanceWeights);
    finalColor = mix(vec4(luminance), finalColor, saturation);
    
    //contrast
    finalColor.rgb = (finalColor.rgb - 0.5f) * (1.0f + contrast) + 0.5f;    
       
    //gamma
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0f / gamma));
    
    //brightness
    finalColor.rgb *= brightness;    
    
    vignette();
    
    fragColor = finalColor;
}
