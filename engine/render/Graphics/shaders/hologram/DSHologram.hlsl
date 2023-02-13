struct VS_OUTPUT
{
    float4x4 modelToWorld : M2W;
    float3 modelPos : POSITION;
    float4 outCol : COLOR;
    float3 modelNorm : NORMAL;
};

struct PatchOut // 3 outer factors and 1 inner factor specifically for “tri” domain
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};


[domain("tri")]
VS_OUTPUT main(PatchOut control, float3 loc : SV_DomainLocation, const OutputPatch<VS_OUTPUT, 3> input)
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