#include "BasicColorStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    float4x4 modelToWorldInv = float4x4(input.modelToWorldInv0, input.modelToWorldInv1, input.modelToWorldInv2, input.modelToWorldInv3);
    
    // Normal is transformed via transpose of the inverse
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv))).xyz;
    float3 modelTan = normalize(mul(float4(input.inTangent, 0.0f), transpose(meshToModelInv))).xyz;
    float3 modelBiTan = normalize(mul(float4(input.inBiTangent, 0.0f), transpose(meshToModelInv))).xyz;
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    // The world inv is transposed at entering the shader
    float3 worldNorm = normalize(mul(float4(modelNorm, 0.0f), modelToWorldInv)).xyz;
    float3 worldTan = normalize(mul(float4(modelTan, 0.0f), modelToWorldInv)).xyz;
    float3 worldBiTan = normalize(mul(float4(modelBiTan, 0.0f), modelToWorldInv)).xyz;
    
    output.outPos = mul(worldPos, worldToClip);
    output.worldPos = worldPos.xyz;
    output.outCol = input.color;
    output.worldNorm = worldNorm;
    output.outTexCoord = input.inTC;
    output.TBN = float3x3(worldTan, worldBiTan, worldNorm);
	return output;
}