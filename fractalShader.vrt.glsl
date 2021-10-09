#version 400
//VERTEX SHADER

// layout location needs to match attribute in glVertexAttribPointer()
// "in" notes that this is input data
// takes a vec3, vPosition_Modelspace is just a name that makes sense
layout(location = 0) in vec3 vPosition_Modelspace;
layout(location = 1) in vec3 vertexColor;
//Arranged in this order:
//scaling, rotation, translation, rotateOrigin, view, projection
uniform mat4 matrices[6];

out vec3 fragmentColor;
out vec3 vPosition_out;

void main() {
    //link vertexPos with gl_Position
    //Generate MVP matrix
    mat4 MVP = matrices[5] * matrices[4] * matrices[3] * matrices[2] * matrices[1] * matrices[0];
    gl_Position = MVP * vec4(vPosition_Modelspace, 1.0);

    //forward color data on to fragment shader
    fragmentColor = vertexColor;
    vPosition_out = vPosition_Modelspace;
}