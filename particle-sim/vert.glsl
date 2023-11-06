#version 330 core

layout (location=0) in vec3 aPos;
out vec4 VertColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    VertColor = vec4(0.8, 0.8, 0.8, 1.0);
}