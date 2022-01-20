#version 460 core
in vec2 TexCoords;
out vec4 FragColor;
//in vec2 col;

uniform sampler2D font;
uniform vec3 textColor;

void main()
{
    FragColor = vec4(textColor, texture(font, TexCoords).r);
}