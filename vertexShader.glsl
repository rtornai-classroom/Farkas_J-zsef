
#version 330 compatibility

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colorIn;

out vec3 color;

uniform float offsetX;
uniform float offsetY;
uniform float lineOffsetY;
uniform int moving;
uniform int collision;

void main()
{
    vec3 pos = position;

    if (moving == 1)
    {
        
        pos.x += offsetX;
        pos.y += offsetY;

        float dist = length(position.xy); // távolság a középponttól
        bool isCenter = dist < 0.001;

        if (collision == 1)
        {
            // ÜTKÖZÉS
            if (isCenter)
                color = vec3(1.0, 0.0, 0.0); // piros közép
            else
                color = vec3(0.0, 1.0, 0.0); // zöld perem
        }
        else
        {
            // NINCS ÜTKÖZÉS
            if (isCenter)
                color = vec3(0.0, 1.0, 0.0); // zöld közép
            else
                color = vec3(1.0, 0.0, 0.0); // piros perem
        }
    }
    else
    {
        
        pos.y += lineOffsetY;
        color = vec3(0.0, 0.0, 1.0);
    }

    gl_Position = vec4(pos, 1.0);
}