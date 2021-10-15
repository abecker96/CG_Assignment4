#version 330 core
// GEOMETRY SHADER
// mainly implemented from https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 fragColor0[];
uniform float geoTimer;
//Matrices arranged in this order:
//[0]scaling, [1]rotation, [2]translation, [3]view, [4]projection
uniform mat4 matrices[5];

out vec3 fragColor;
out vec3 vNormal;

// Returns the normal for a CCW triangle
vec3 getNormal(vec4 v0, vec4 v1, vec4 v2)
{
   vec3 a = vec3(v0) - vec3(v1);
   vec3 b = vec3(v2) - vec3(v1);
   return normalize(cross(b, a));
}  

// Breathe function
// Just translates each vertex along a vector (in this instance the normal)
// According to sin() of a timer
vec4 breathe(vec4 position, vec3 normal)
{
    vec3 direction = normal * (sin(geoTimer)+1.05) * 0.01;
    return position + vec4(direction, 0.0);
}

void main() {
    // Generate M/VP matrix
    // Divide up like this to get normals after object to world transforms
    // but before view/projection
    mat4 M = matrices[2] * matrices[1] * matrices[0];
    mat4 VP = matrices[4] * matrices[3];

    // Object to world transform
    vec4 v0 = M * gl_in[0].gl_Position;
    vec4 v1 = M * gl_in[1].gl_Position;
    vec4 v2 = M * gl_in[2].gl_Position;

    // calculate per-face normal
    // If anybody sees this, I understand that this is not the way to do this
    // Just calculate the normals once on the CPU, then pass to shaders as an attribute
    // With that being said, this is a workaround to get per-face normals
    // using an IBO
    vec3 normal_face = getNormal(v0, v1, v2);

    // Apply View/Projection transforms and breathing effect
    gl_Position = VP * breathe(v0, normal_face);
    fragColor = fragColor0[0];
    vNormal = normal_face;
    EmitVertex();

    gl_Position = VP * breathe(v1, normal_face);
    fragColor = fragColor0[1];
    // vNormal = normal_face;   // Don't need to do this for every vertex
                                // But clarity is nice
    EmitVertex();

    gl_Position = VP * breathe(v2, normal_face);
    fragColor = fragColor0[2];
    // vNormal = normal_face;
    EmitVertex();

    EndPrimitive();
}