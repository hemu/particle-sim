#version 330 core

uniform vec3 iResolution;

in vec4 vertColor;
out vec4 FragColor;

vec3 rgb(float r, float g, float b) {
	return vec3(r / 255.0, g / 255.0, b / 255.0);
}

// Draw a circle at vec2 `pos` with radius `rad` and color `color`.
vec4 circle(vec2 uv, vec2 pos, float rad, vec3 color) {
	float d = length(pos - uv) - rad;
	float t = clamp(d, 0.0, 1.0);
	return vec4(color, 1.0 - t);
}

void main() {
	vec2 uv = gl_FragCoord.xy;
	vec2 center = iResolution.xy * 0.5;
	float radius = 0.01 * iResolution.y;
	
	vec3 circleColor = rgb(225.0, 95.0, 60.0);

	FragColor = circle(uv, center, radius, circleColor);
}
