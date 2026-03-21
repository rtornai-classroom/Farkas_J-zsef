#version 330 compatibility

in vec3 color;
out vec4 fragColor;

void main()
{
    fragColor = vec4(color,1.0);
}

//outColor = vec4(0.0, 0.0, 1.0, 1.0);