#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform dvec2 center;
uniform double scale;
uniform int iter;
uniform vec3 color;
uniform float colorCount;

void main()
{
    dvec2 z, c;
    c.x = (TexCoord.y - 0.5lf) * scale - center.x;
    c.y = (TexCoord.x - 0.5lf) * scale - center.y;

    int i;
    z = c;

    for (i = 0; i < iter; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;

        if ((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

    FragColor = i == iter ? vec4(color, 1.0) : texture(texture1, vec2(float(i) / colorCount));
}