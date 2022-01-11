#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec2 center;
uniform float scale;
uniform int iter;
uniform vec3 color;
uniform float colorCount;
uniform vec2 constant;

void main()
{
    vec2 z;
    z.x = (TexCoord.y - 0.5) * scale - center.x;
    z.y = (TexCoord.x - 0.5) * scale - center.y;

    int i;
    for (i = 0; i < iter; i++)
    {
        float x = (z.x * z.x - z.y * z.y) + constant.x;
        float y = (z.y * z.x + z.x * z.y) + constant.y;

        if ((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

    FragColor = i == iter ? vec4(color, 1.0) : texture(texture1, vec2(float(i) / colorCount));
}