#include "VisNormalStructs.hlsli"

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    float4x4 modelToWorldInv = float4x4(input.modelToWorldInv0, input.modelToWorldInv1, input.modelToWorldInv2, input.modelToWorldInv3);

    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv))).xyz;
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    float3 worldNorm = normalize(mul(float4(modelNorm, 0.0f), modelToWorldInv)).xyz;

    output.worldPos = worldPos.xyz;
    output.worldNorm = worldNorm;
    return output;
}