#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec2 center;
uniform float scale;

uniform vec2[10] constants;
uniform int constantCount;

void main()
{
    vec2 pos = vec2((TexCoord.y - 0.5) * scale - center.x, (TexCoord.x - 0.5) * scale - center.y);

    FragColor = texture(texture1, vec2(length(vec2(0, 0) - pos))) * 0.5;
    FragColor.w *= 2;

    for (int i = 0; i < constantCount; i++)
    {
        vec2 dif = pos - constants[i];
        float dist = length(dif);
        if (dist < (0.007 * scale) && dist > (0.004 * scale))
        {
            FragColor = texture(texture1, vec2(float(i) / constantCount)) * 2;
            FragColor.w *= 0.5;
            return;
        }
    }
}