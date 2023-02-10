#include "HologramInclude.hlsli"


struct VS_OUTPUT
{
    float3 modelPos : POSITION0;
    float3 worldPos : POSITION1;
    float4 outCol : COLOR;
    float3 modelNorm : NORMAL0;
    float3 worldNorm : NORMAL1;
};

struct GS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 modelPos : POS;
    float4 outCol : COLOR;
    float3 modelNorm : NORM;
};


[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> output)
{
    for (uint i = 0; i < 3; ++i)
    {
        GS_OUTPUT vertex;
        vertex.outPos = mul(float4(input[i].worldPos, 1.0f), worldToClip);
        vertex.modelPos = input[i].modelPos;
        vertex.outCol = input[i].outCol;
        vertex.modelNorm = input[i].modelNorm;
        output.Append(vertex);

    }
}