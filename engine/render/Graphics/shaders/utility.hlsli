#ifndef UTILITY_HLSLI
#define UTILITY_HLSLI

float remap(float low1, float high1, float low2, float high2, float value)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

float2 nonZeroSign(float2 v)
{
    return float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

float2 packOctahedron(float3 v)
{
    float2 p = v.xy / (abs(v.x) + abs(v.y) + abs(v.z));
    return v.z <= 0.0 ? (float2(1.0, 1.0) - abs(p.yx)) * nonZeroSign(p) : p;
}

float3 unpackOctahedron(float2 oct)
{
    float3 v = float3(oct, 1.0 - abs(oct.x) - abs(oct.y));
    if (v.z < 0)
        v.xy = (float2(1, 1) - abs(v.yx)) * nonZeroSign(v.xy);
    return normalize(v);
}

bool IsNaN(float x)
{
    return (asuint(x) & 0x7fffffff) > 0x7f800000;
}

#endif // UTILITY_HLSLI