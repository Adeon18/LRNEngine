#include "HologramInclude.hlsli"
#include "HologramStructs.hlsli"

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv)));
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    //float3 offset = vertexDistortion(modelPos.xyz, modelNorm);
    //modelPos += float4(offset, 1.0f);
    
    //float4 worldPos = mul(modelPos, modelToWorld);
    //float4 worldNorm = mul(float4(modelNorm, 1.0f), modelToWorld);
    
    //output.outPos = mul(worldPos, worldToClip);
    output.modelToWorld = modelToWorld;
    output.modelPos = modelPos.xyz;
    output.outCol = input.color;
    output.modelNorm = modelNorm;
    return output;
}