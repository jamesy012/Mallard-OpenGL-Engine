#version 410
//for the depth converter
//http://www.ozone3d.net/blogs/lab/20090206/how-to-linearize-the-depth-value/
in vec2 vTexCoord;

out vec4 fragColor;

uniform vec2 resolution;

//depth texture
uniform sampler2D TexDiffuse1;

uniform float focusDistance = 95f;
uniform float farDof = 6f;
uniform float nearDof = 6f;
uniform float falloff = 2.7f;

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
    
    float sFocusDistance = focusDistance/f;
    float sFarDof = farDof/f;
    float sNearDof = nearDof/f;

     
    if(z > sFocusDistance){
       dofStrength = 1-((sFocusDistance+sFarDof)/(z));
    }else{
       dofStrength = 1-((z)/(sFocusDistance-sNearDof));
    }
    
    dofStrength = clamp(dofStrength,0.0f,0.99f);
    dofStrength *= falloff;
    
    //if(z > focusDistance + farDof){
    //    dofStrength = 1;
    //}
    fragColor = vec4(dofStrength,dofStrength,dofStrength,1);
     
}
