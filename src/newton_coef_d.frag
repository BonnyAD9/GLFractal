#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform dvec2 center;
uniform double scale;
uniform int iter;
uniform vec3 color;

uniform vec2[10] roots;
uniform int rootCount;

uniform vec2[11] coefs;
uniform int coefCount;

dvec2 newtonRaphson(dvec2 z);
dvec2 cMul(dvec2 a, dvec2 b);
dvec2 cDiv(dvec2 a, dvec2 b);
dvec2 fun(dvec2 z);
dvec2 deriv(dvec2 z);

void main()
{
    if (rootCount == 0)
    {
        FragColor = vec4(color, 1.0);
        return;
    }
    dvec2 z = dvec2((TexCoord.y - 0.5lf) * scale - center.x, (TexCoord.x - 0.5lf) * scale - center.y);
    dvec2 zCopy = z;
	int c = 0;
	int i;
    for (i = 1; i < iter - 1; i++)
    {
        z = newtonRaphson(z);
		double dist = length(roots[0] - z);
		c = 0;
		for (int j = 1; j < rootCount; j++)
		{
			double newDist = length(roots[j] - z);
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
    double zDist = length(zCopy - roots[c]);
    if (zDist < (0.007 * scale) && zDist > (0.004 * scale))
    {
        col = vec4(1.0 - col.x, 1.0 - col.y, 1.0 - col.z, 1.0);
    }
    FragColor = col;
}

dvec2 newtonRaphson(dvec2 z)
{
    return z - cDiv(fun(z), deriv(z));
}

dvec2 cMul(dvec2 a, dvec2 b)
{
    return dvec2(
        a.x * b.x - a.y * b.y,
        a.x * b.y + a.y * b.x
    );
}

dvec2 cDiv(dvec2 a, dvec2 b)
{
    double div = b.x * b.x + b.y * b.y;
    return dvec2(
        (a.x * b.x + a.y * b.y) / div,
        (a.y * b.x - a.x * b.y) / div
    );
}

dvec2 fun(dvec2 z)
{
    dvec2 res = coefs[0];
    for (int i = 1; i < coefCount; i++)
    {
        res = cMul(res, z) + coefs[i];
    }
    return res;
}

dvec2 deriv(dvec2 z)
{
    int coefc = coefCount - 1;
    dvec2 res = coefs[0] * coefc;
    for (int i = 1; i < coefc; i++)
    {
        res = cMul(res, z) + (coefc - i) * coefs[i];
    }
    return res;
}