#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec2 center;
uniform float scale;
uniform int iter;
uniform vec3 color;

uniform vec2[10] roots;
uniform int rootCount;

uniform vec2[11] coefs;
uniform int coefCount;

vec2 newtonRaphson(vec2 z);
vec2 cMul(vec2 a, vec2 b);
vec2 cDiv(vec2 a, vec2 b);
vec2 fun(vec2 z);
vec2 deriv(vec2 z);

void main()
{
    if (rootCount == 0)
    {
        FragColor = vec4(color, 1.0);
        return;
    }
    vec2 z = vec2((TexCoord.y - 0.5) * scale - center.x, (TexCoord.x - 0.5) * scale - center.y);
    vec2 zCopy = z;
	int c = 0;
	int i;
    for (i = 1; i < iter + 1; i++)
    {
        z = newtonRaphson(z);
		float dist = length(roots[0] - z);
		c = 0;
		for (int j = 1; j < rootCount; j++)
		{
			float newDist = length(roots[j] - z);
			if (newDist < dist)
			{
				dist = newDist;
				c = j;
			}
		}
		if (dist < 0.0001)
			break;
    }

    bool isC = false;

    vec4 col = i >= iter ? vec4(0) : texture(texture1, vec2(float(c) / rootCount)) * (1.0 - (i / float(iter)));
	col.w = 1.0;
    float zDist = length(zCopy - roots[c]);
    if (zDist < (0.007 * scale) && zDist > (0.004 * scale))
    {
        col = vec4(1.0 - col.x, 1.0 - col.y, 1.0 - col.z, 1.0);
    }
    FragColor = col;
}

vec2 newtonRaphson(vec2 z)
{
    return z - cDiv(fun(z), deriv(z));
}

vec2 cMul(vec2 a, vec2 b)
{
    return vec2(
        a.x * b.x - a.y * b.y,
        a.x * b.y + a.y * b.x
    );
}

vec2 cDiv(vec2 a, vec2 b)
{
    float div = b.x * b.x + b.y * b.y;
    return vec2(
        (a.x * b.x + a.y * b.y) / div,
        (a.y * b.x - a.x * b.y) / div
    );
}

vec2 fun(vec2 z)
{
    vec2 res = coefs[0];
    for (int i = 1; i < coefCount; i++)
    {
        res = cMul(res, z) + coefs[i];
    }
    return res;
}

vec2 deriv(vec2 z)
{
    int coefc = coefCount - 1;
    vec2 res = coefs[0] * coefc;
    for (int i = 1; i < coefc; i++)
    {
        res = cMul(res, z) + (coefc - i) * coefs[i];
    }
    return res;
}