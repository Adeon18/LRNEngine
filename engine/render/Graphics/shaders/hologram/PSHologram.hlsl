#include "HologramInclude.hlsli"
#include "HologramStructs.hlsli"


#define DEBUG 0

float4 main(GS_OUTPUT inp) : SV_TARGET
{
#if DEBUG
    return float4(inp.modelNorm, 1.0f);
#else
    return float4(colorDistortion(inp.modelPos, inp.modelNorm, inp.outCol), 1.0f);
#endif
}