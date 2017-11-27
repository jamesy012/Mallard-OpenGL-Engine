#version 410
//for the depth converter
//http://www.ozone3d.net/blogs/lab/20090206/how-to-linearize-the-depth-value/
in vec2 vTexCoord;

out vec4 fragColor;

uniform vec2 resolution;

//depth texture
uniform sampler2D TexDiffuse1;

void main() {
    vec2 uvOffset = 5/resolution;
    float average;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(-uvOffset.x, -uvOffset.y)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(0,           -uvOffset.y)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(uvOffset.x,  -uvOffset.y)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(-uvOffset.x, 0)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(0,           0)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(uvOffset.x,  0)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(-uvOffset.x, uvOffset.y)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(0,           uvOffset.y)).x;
    //average += texture2D(TexDiffuse1, vTexCoord + vec2(uvOffset.x,  uvOffset.y)).x;
    //average /= 9;
    average = texture2D(TexDiffuse1, vTexCoord + vec2(0,0)).x;
    float f= 1000.0;
    float n = 0.1;
    float z = (2 * n) / (f + n - average * (f - n));
    
    float dofStrength = 0;
    
    float focusDistance = 0.1f;
    float farDof = 0.03f;
    float nearDof = 0.011f;

    float falloff = 3f;

     
    if(z > focusDistance){
       dofStrength = 1-((focusDistance+farDof)/(z));
    }else{
       dofStrength = 1-((z)/(focusDistance-nearDof));
    }
    dofStrength *= falloff;
    dofStrength = clamp(dofStrength,0.0f,0.99f);
    
    
    //if(z > focusDistance + farDof){
    //    dofStrength = 1;
    //}
    fragColor = vec4(dofStrength,dofStrength,dofStrength,1);
     
}
