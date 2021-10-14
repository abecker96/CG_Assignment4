#version 330 core
// GEOMETRY SHADER
// mainly learned from https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 vPosition_modelspace0[];
uniform float geoTimer;

out vec3 vPosition_modelspace1;

vec3 getNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 0.10;
    vec3 direction = normal * ((sin(geoTimer) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
} 

void main() {
    vec3 normal_face = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[0];
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[1];
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal_face);
    vPosition_modelspace1 = vPosition_modelspace0[2];
    EmitVertex();

    EndPrimitive();
}