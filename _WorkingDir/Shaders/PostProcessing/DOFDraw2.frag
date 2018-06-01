#version 410
//for the gaussian blur
//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
in vec2 vTexCoord;

out vec4 fragColor;

//rgb texture
uniform sampler2D TexDiffuse1;
//depth texture generated from DOFGen
uniform sampler2D depthTexture;


void main() {

    if(vTexCoord.x < 0.5){
        fragColor = vec4(texture2D(TexDiffuse1,vTexCoord).rgb, 1.0);
    }else{
        float blur = texture2D(depthTexture,vTexCoord).r;
        if(blur > 1.5f){
            fragColor = vec4(texture2D(TexDiffuse1,vTexCoord).rgb, 1.0);
        }else{
            fragColor = vec4(texture2D(depthTexture,vTexCoord).rrr, 1.0);
        }
    }
}
