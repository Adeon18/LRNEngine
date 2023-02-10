#include "HologramInclude.hlsli"

struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 modelPos : POS;
    float4 outCol : COLOR;
    float3 modelNorm : NORM;
};

#define DEBUG 0

float4 main(PS_INPUT inp) : SV_TARGET
{
#if DEBUG
    return float4(inp.outNorm, 1.0f);
#else
    return float4(colorDistortion(inp.modelPos, inp.modelNorm, inp.outCol), 1.0f);
#endif
}