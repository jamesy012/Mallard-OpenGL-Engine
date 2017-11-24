#version 410

//TODO: add preprocessor defines to this, to only include what is needed

/** COMMON */
in vec2 vTexCoord;
out vec4 fragColor;
uniform sampler2D TexDiffuse1;


uniform float brightness = 1.0f;
uniform float contrast = -0.1f;
uniform float saturation = 1.2f;
uniform vec3 luminanceWeights = vec3(0.2126f,0.5152f,0.1722f);
uniform vec3 gamma = vec3(1.25f);

vec3 vHSV = vec3(180,0.5,10);

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 gaussianBlur(vec4 a_Input){
    float offset = 0.003;
    vec2 offsets[9];
    offsets = vec2[](
    vec2(-offset, offset),  // top-left
    vec2(0.0f,    offset),  // top-center
    vec2(offset,  offset),  // top-right
    vec2(-offset, 0.0f),    // center-left
    vec2(0.0f,    0.0f),    // center-center
    vec2(offset,  0.0f),    // center-right
    vec2(-offset, -offset), // bottom-left
    vec2(0.0f,    -offset), // bottom-center
    vec2(offset,  -offset));// bottom-right
    float kernel[9];
    kernel = float[](
    0.0625, 0.125, 0.0625,
    0.125,  0.25,  0.125,
    0.0625, 0.125, 0.0625);
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
    vec3 color = vec3(texture(TexDiffuse1, vTexCoord + offsets[i]));
    col += color * kernel[i];
    }
    return vec4(col, 1.0);
}
    
void main() {
    fragColor = texture(TexDiffuse1, vTexCoord);
    
    //saturation
    float luminance = dot(fragColor.rgb, luminanceWeights);
    fragColor = mix(vec4(luminance), fragColor, saturation);
    
    //fragColor = gaussianBlur(fragColor);
    
    //contrast
    fragColor.rgb = (fragColor.rgb - 0.5f) * (1.0f + contrast) + 0.5f;    
       
    //gamma
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0f / gamma));
    
    //brightness
    fragColor.rgb *= brightness;
    
    // average brightness of this frame
    //vec3 Color = texture2D( TexDiffuse1, vec2(0.5, 0.5) ).rgb;
    //float AverageBrightness = clamp( max(max(Color.r, Color.g), Color.b), 0.3, 0.7 );
    //
    //// exposure of previous frame
    //float PreviousExposure = texture2D( ... );
    //
    //// compute exposure
    //float Exposure = lerp( PreviousExposure , 0.5/AverageBrightness, 0.5 );
    //
    //// perform tone-mapping
    //Scene.rgb *= Exposure*( Exposure/u_MaximumBrightness+1.0 )/( Exposure+1.0 );
    
}
