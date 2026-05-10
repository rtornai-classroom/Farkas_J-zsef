#version 430
in vec3 vNormal;
in vec3 vPos;
out vec4 fragColor;

uniform vec3 lightPos;
uniform bool isSun;
uniform bool useLighting;

void main(void) {
    if (isSun) {
        fragColor = vec4(1.0, 1.0, 0.5, 1.0); // Világító sárga gömb
    } else {
        vec3 color = vec3(1.0, 1.0, 1.0); // Fehér kockák
        if (useLighting) {
            vec3 norm = normalize(vNormal);
            vec3 lightDir = normalize(lightPos - vPos);
            float diff = max(dot(norm, lightDir), 0.2);
            color = color * diff;
        }
        fragColor = vec4(color, 1.0);
    }
}
