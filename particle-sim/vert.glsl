#version 330 core

layout (location=0) in vec3 aPos;
out vec4 vertColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertColor = vec4(0.2, 0.6, 0.1, 1.0);
}