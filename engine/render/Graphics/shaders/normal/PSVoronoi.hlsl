#define VORONOI 1

#define PI 3.14159

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
    float iTime;
};

// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org/
// Ported to HLSL by Ostap Trush

float2 hash(float2 p)
{
    //p = mod(p, 4.0); // tile
    p = float2(dot(p, float2(127.1, 311.7)),
        dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 18.5453);
}

// return distance, and cell id
float2 voronoi(in float2 x)
{
    float2 n = floor(x);
    float2 f = frac(x);

    float3 m = float3(8.0f, 8.0f, 8.0f);
    for (int j = -1; j <= 1; j++)
        for (int i = -1; i <= 1; i++)
        {
            float2 g = float2(float(i), float(j));
            float2 o = hash(n + g);
            //vec2  r = g - f + o;
            float2 r = g - f + (0.5 + 0.5 * sin(iTime + 2 * PI * o));
            float d = dot(r, r);
            if (d < m.x)
            {
                m = float3(d, o);
            }
        }

    return float2(sqrt(m.x), m.y + m.z);
}
// Voronoi Shader end

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float3 worldNorm : NORM;
    float2 outTexCoord : TEXCOORD;
    float4 outTime : TIME;
    float3x3 TBN : TBN;
};

float4 main(PS_INPUT inp) : SV_TARGET
{
    float2 p = inp.outPos.xy / max(iResolution.x, iResolution.y);

	// computer voronoi patterm
    float2 c = voronoi((14.0 + 6.0 * sin(0.2 * iTime)) * p);

	// colorize
	float3 col = 0.5 + 0.5 * cos(c.y * 6.2831 + float3(0.0, 1.0, 2.0));
	col *= clamp(1.0 - 0.4 * c.x * c.x, 0.0, 1.0);
	col -= (1.0 - smoothstep(0.08, 0.09, c.x));

#if VORONOI
	float4 outCol = float4(col, 1.0);
#else
    float4 outCol = inp.outTime;
#endif
	return outCol;
}