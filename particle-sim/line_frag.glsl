#version 330 core

uniform vec3 iResolution;

in vec4 vertColor;
out vec4 FragColor;

vec3 rgb(float r, float g, float b) {
	return vec3(r / 255.0, g / 255.0, b / 255.0);
}

void main() {
	FragColor = vec4(rgb(225.0, 95.0, 60.0), 1.0);
}

