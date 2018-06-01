#version 410
//for the depth converter
//http://www.ozone3d.net/blogs/lab/20090206/how-to-linearize-the-depth-value/
in vec2 vTexCoord;

out vec4 fragColor;

//depth texture
uniform sampler2D TexDiffuse1;

uniform float focusDistance =  15f;
uniform float farDof = 1.0f;
uniform float nearDof = 0.0f;
//how quick does it fade to nothing, higher is quicker
uniform float falloff = 5.0f;

//note results vary wildly when you change these
uniform float nearPlane = 0.1f;
uniform float farPlane = 100.0f;

void main() {
    float distance = texture2D(TexDiffuse1, vTexCoord).x;

    float z = (2 * nearPlane) / (farPlane + nearPlane - distance * (farPlane - nearPlane));
    
    float dofStrength = 0;
    
    //float sFocusDistance = focusDistance/farPlane;
    float sFarDof = (focusDistance+farDof)/farPlane;
    float sNearDof = (focusDistance-nearDof)/farPlane;

    
     
    if(z > sFarDof){
       dofStrength = ((sFarDof)/(z));
    }else if (z < sNearDof) {
       dofStrength = ((z)/(sNearDof));
    }
    
    dofStrength = clamp(1-dofStrength,0.0f,1.0f);
    dofStrength *= falloff;
    //dofStrength = pow(dofStrength,0.5);
    
    fragColor = vec4(dofStrength,dofStrength,dofStrength,1);
}
