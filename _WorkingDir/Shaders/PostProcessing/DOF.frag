#version 410
//for the gaussian blur
//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D normalTex;
uniform sampler2D blurredTex;
uniform sampler2D depthTex;


uniform float focusDistance =  5f;
uniform float farDof = 1.0f;
uniform float nearDof = 1.0f;
//how quick does it fade to nothing, higher is quicker
uniform float falloff = 15.0f;

//note results vary wildly when you change these
uniform float nearPlane = 0.1f;
uniform float farPlane = 100.0f;

uniform float minStrength = 0.2f;

float pingpong(const float a_Value,const float a_Max){
    float newVal = mod(a_Value, (a_Max*2));
    if(newVal > a_Max){
        newVal = a_Max + (a_Max-newVal);
    }
    return newVal;
}

float depthStrength(){
    float distance = texture2D(depthTex, vTexCoord).x;

    float z = (2 * nearPlane) / (farPlane + nearPlane - distance * (farPlane - nearPlane));
    
    float dofStrength = 1;
    
    float sFarDof = (focusDistance+farDof)/farPlane;
    float sNearDof = (focusDistance-nearDof)/farPlane;    
     
    if(z > sFarDof){
       //dofStrength = ((sFarDof)/(z));
        dofStrength = ((sFarDof)/(z));
    }else if (z < sNearDof) {
       dofStrength = ((z)/(sNearDof));
    }    
    

    dofStrength = clamp(1-dofStrength,0.0f,1.0f);
    dofStrength *= falloff;
    dofStrength =clamp(1-dofStrength,0.0f,1.0f);
    return mix(minStrength, 1.0f,dofStrength);
}

void main() {    
    float dofStrength = depthStrength();
    
    vec3 texCol = mix(texture2D(normalTex, vTexCoord),texture2D(blurredTex, vTexCoord),1-dofStrength).rgb;
    //texCol = vec3(dofStrength,dofStrength,dofStrength);
    fragColor = vec4(texCol,1);
    
    /*
    if(vTexCoord.x < 0.5){
        //dofStrength = pingpong(vTexCoord.x,0.05);
        fragColor = vec4(dofStrength,dofStrength,dofStrength,1);
    }
*/
}
