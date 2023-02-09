cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};


float3 colorCompute(float3 pos, float3 normal, float4 col)
{
    float3 toCamera = normalize(iCameraPosition.xyz - pos);
    float contourGlow = pow(1.0 - abs(dot(normal, toCamera)), 2);

    float3 color = col.xyz * min(1.0f, contourGlow);
    
    return color;
}

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float4 outCol : COLOR;
    float3 outNorm : NORM;
};

#define DEBUG 1

float4 main(PS_INPUT inp) : SV_TARGET
{
#if DEBUG
    return inp.outCol;
#else
    return float4(colorCompute(inp.worldPos, inp.outNorm, inp.outCol), 1.0f);
#endif
}