#include "HologramStructs.hlsli"

[domain("tri")]
VS_OUTPUT main(PATCH_OUTPUT control, float3 loc : SV_DomainLocation, const OutputPatch<VS_OUTPUT, 3> input)
{
    VS_OUTPUT result;
	// manual barycentric interpolation
    result.modelPos = loc.x * input[0].modelPos +
				     loc.y * input[1].modelPos +
				     loc.z * input[2].modelPos;
    result.modelNorm = loc.x * input[0].modelNorm +
				     loc.y * input[1].modelNorm +
				     loc.z * input[2].modelNorm;
    
    result.modelToWorld = input[0].modelToWorld;
    result.outCol = input[0].outCol;
    
    
    return result;
}