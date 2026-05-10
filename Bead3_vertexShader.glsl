#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 vNormal;
out vec3 vPos;
out vec2 vTex;

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;

void main(void) {
    vPos = vec3(model_matrix * vec4(position, 1.0));
    vNormal = mat3(transpose(inverse(model_matrix))) * normal;
    vTex = texCoord;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}
