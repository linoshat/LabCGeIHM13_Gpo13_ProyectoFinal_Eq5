#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vDir;

uniform mat4 projection;
uniform mat4 view;

void main() {
    // Quita la traslación para que el skybox sea "infinito"
    mat4 viewNoTrans = mat4(mat3(view));
    vDir = aPos;
    gl_Position = projection * viewNoTrans * vec4(aPos, 1.0);
}
