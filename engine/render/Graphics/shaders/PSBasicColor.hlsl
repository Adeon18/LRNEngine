cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

static const float MIN_DOT = 0.0001f;

//! Basic visualization dot product shader
float3 colorCompute(float3 pos, float3 normal, float4 col)
{
    float3 toCamera = normalize(iCameraPosition.xyz - pos);

    float3 color = col.xyz * max(MIN_DOT, dot(normal, toCamera));
    
    return color;
}

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float4 outCol : COLOR;
    float3 worldNorm : NORM;
};

#define DEBUG 0

float4 main(PS_INPUT inp) : SV_TARGET
{
#if DEBUG
    return inp.outCol;
#else
    return float4(colorCompute(inp.worldPos, inp.worldNorm, inp.outCol), 1.0f);
#endif
}