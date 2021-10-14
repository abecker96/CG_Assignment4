#version 330 core
// GEOMETRY SHADER
// mainly learned from https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec3 vPosition_modelspace0[];
uniform float geoTimer;
//Arranged in this order:
//scaling, rotation, translation, view, projection matrices
uniform mat4 matrices[5];

out vec3 vPosition_modelspace1;
out vec3 vNormal;

vec3 getNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(b, a));
}  

vec4 explode(vec4 position, vec3 normal)
{
    vec3 direction = normal * (sin(geoTimer)+1) * 0.01; 
    return position + vec4(direction, 0.0);
}

void main() {
    vec3 normal_face = getNormal();
    //link vertexPos with gl_Position
    //Generate MVP matrix
    mat4 MVP = matrices[4] * matrices[3] * matrices[2] * matrices[1] * matrices[0];

    gl_Position = MVP * explode(gl_in[0].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[0];
    vNormal = normal_face;
    EmitVertex();

    gl_Position = MVP * explode(gl_in[1].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[1];
    vNormal = normal_face;
    EmitVertex();

    gl_Position = MVP * explode(gl_in[2].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[2];
    vNormal = normal_face;
    EmitVertex();

    EndPrimitive();
}