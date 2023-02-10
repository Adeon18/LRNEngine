#include "HologramInclude.hlsli"


struct VS_OUTPUT
{
    float4x4 modelToWorld : M2W;
    float3 modelPos : POSITION;
    float4 outCol : COLOR;
    float3 modelNorm : NORMAL;
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
    float3 triangleNorm = (input[0].modelNorm + input[1].modelNorm + input[2].modelNorm) / 3.0f;
    
    
    for (uint i = 0; i < 3; ++i)
    {
        float3 vetriceOffset = vertexDistortion(input[i].modelPos, triangleNorm);
        
        float3 newModelPos = input[i].modelPos + vetriceOffset;
        
        float4 worldPos = mul(float4(newModelPos, 1.0f), input[i].modelToWorld);
        
        GS_OUTPUT vertex;
        vertex.outPos = mul(worldPos, worldToClip);
        vertex.modelPos = input[i].modelPos;
        vertex.outCol = input[i].outCol;
        vertex.modelNorm = input[i].modelNorm;
        output.Append(vertex);

    }
}