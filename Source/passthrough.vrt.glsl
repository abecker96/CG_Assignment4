#version 330 core
//VERTEX SHADER

// layout location needs to match attribute in glVertexAttribPointer()
// "in" notes that this is input data
// takes a vec3, vPosition_Modelspace is just a name that makes sense
layout(location = 0) in vec3 vPosition_Modelspace;
layout(location = 1) in vec3 vertexColor;

out vec3 fragColor0;

void main() {
    // link vertex position with passed data
    gl_Position = vec4(vPosition_Modelspace, 1.0);

    //forward color data on to fragment shader
    fragColor0 = vertexColor;
}