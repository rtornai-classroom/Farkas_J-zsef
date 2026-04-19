#version 330

in	vec2	varyingPosition;
out vec4	outColor;

void main(void) {
	outColor = vec4(0.5 + varyingPosition.x, 0.5 + varyingPosition.y, 0.5, 1.0);
}
