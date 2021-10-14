#version 330 core
//FRAGMENT SHADER

// in vec3 fragmentColor;
in vec3 vPosition_modelspace1;
in vec3 vNormal;

uniform vec3 wireframeColor;
uniform int colorType;

out vec4 color;

void main() {
    // if(colorType == 0)
    // {
    //     color = vec4(wireframeColor, 1);
    // }
    // else if(colorType == 1)
    // {
    //     color = vec4(fragmentColor, 1);
    // }
    // else
    // {
    //     color = vec4(
    //         sin(vPosition_out.x + colorTimer)/2 + 0.5, 
    //         cos(vPosition_out.y + colorTimer)/2 + 0.5, 
    //         cos(vPosition_out.z + colorTimer)/2 + 0.5, 
    //         1
    //     );
    // }
    if(colorType == 0)
    {
        color = vec4(vNormal, 1);
    }
    else
    {
        color = vec4(vPosition_modelspace1, 1);
    }
}