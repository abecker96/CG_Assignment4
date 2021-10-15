#version 330 core
//FRAGMENT SHADER

in vec3 fragColor;

uniform int colorType;
uniform vec3 wireframeColor;

out vec4 color;

void main() {
    //Dynamically switch between color types
    if(colorType == 0)
    {
        color = vec4(wireframeColor, 1);
    }
    else
    {
        color = vec4(fragColor, 1);
    }
}