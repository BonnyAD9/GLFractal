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
    vec2 z, c;
    c.x = (TexCoord.y - 0.5) * scale - center.x;
    c.y = (TexCoord.x - 0.5) * scale - center.y;

    int i;
    z = c;

    for (i = 0; i < iter; i++)
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if ((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

    vec2 dif = c - constant;
    float dist = length(dif);
    vec4 col = i == iter ? vec4(color, 1.0) : texture(texture1, vec2(float(i) / colorCount));
    if (dist < (0.007 * scale) && dist > (0.004 * scale))
    {
        col = vec4(1.0 - col.x, 1.0 - col.y, 1.0 - col.z, 1.0);
        col.x = 1.0;
    }
    FragColor = col;
}