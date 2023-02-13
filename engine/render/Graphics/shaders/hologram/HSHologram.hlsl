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


PatchOut mainPatch(InputPatch<VS_OUTPUT, 3> input, uint patchId : SV_PrimitiveID)
{
    PatchOut output;
    output.EdgeFactors[0] = 2;
    output.EdgeFactors[1] = 2;
    output.EdgeFactors[2] = 2;
    output.InsideFactor = 2;
    return output;
}