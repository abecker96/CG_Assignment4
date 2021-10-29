#version 330 core
// GEOMETRY SHADER
// mainly implemented from https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 fragColor0[];

out vec3 fragColor;

void main() {
    // Apply View/Projection transforms and breathing effect
    gl_Position = gl_in[0].gl_Position;
    fragColor = fragColor0[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fragColor = fragColor0[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    fragColor = fragColor0[2];
    EmitVertex();

    EndPrimitive();
}