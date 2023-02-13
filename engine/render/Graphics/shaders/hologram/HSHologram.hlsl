#include "HologramStructs.hlsli"

[outputcontrolpoints(3)] // doesn’t have to match InputPatch argument size (3)
[domain("tri")]
[outputtopology("triangle_cw")]
[partitioning("integer")]
[patchconstantfunc("mainPatch")]
VS_OUTPUT main(InputPatch<VS_OUTPUT, 3> input,
	uint pointId : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
    return input[pointId]; // input size matches the number specified in the set PATCHLIST Topology
}

#define WORLD_SPACE_TESSELATION_OFF 0


PATCH_OUTPUT mainPatch(InputPatch<VS_OUTPUT, 3> input, uint patchId : SV_PrimitiveID)
{
    PATCH_OUTPUT output;
   
    float3 worldPosV0 = mul(float4(input[0].modelPos, 1.0f), input[0].modelToWorld).xyz;
    float3 worldPosV1 = mul(float4(input[1].modelPos, 1.0f), input[1].modelToWorld).xyz;
    float3 worldPosV2 = mul(float4(input[2].modelPos, 1.0f), input[2].modelToWorld).xyz;
    
    int innerFactor = 10 * (length(worldPosV0 - worldPosV1) + length(worldPosV1 - worldPosV2) + length(worldPosV0 - worldPosV2)) / 3.0f;
    
#if WORLD_SPACE_TESSELATION_OFF
    output.EdgeFactors[0] = 5;
    output.EdgeFactors[1] = 5;
    output.EdgeFactors[2] = 5;
    output.InsideFactor = 5;
#else 
    output.EdgeFactors[0] = max(int(length(worldPosV0 - worldPosV1) * 10), 1);
    output.EdgeFactors[1] = max(int(length(worldPosV1 - worldPosV2) * 10), 1);
    output.EdgeFactors[2] = max(int(length(worldPosV0 - worldPosV2) * 10), 1);
    output.InsideFactor = max(innerFactor, 1);
#endif
    return output;
}