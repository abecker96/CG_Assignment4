#version 330 core
//FRAGMENT SHADER

in vec3 fragColor;
in vec3 vNormal;

uniform int colorType;

out vec4 color;

void main() {
    //Dynamically switch between color types
    if(colorType == 0)
    {
        color = vec4(vNormal, 1);
    }
    else
    {
        color = vec4(fragColor, 1);
    }
}