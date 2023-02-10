#include "HologramInclude.hlsli"

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNorm : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float3 inTC : TEXCOORD;
    float4 modelToWorld0 : MODEL2WORLD0;
    float4 modelToWorld1 : MODEL2WORLD1;
    float4 modelToWorld2 : MODEL2WORLD2;
    float4 modelToWorld3 : MODEL2WORLD3;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float3 modelPos : POSITION0;
    float3 worldPos : POSITION1;
    float4 outCol : COLOR;
    float3 modelNorm : NORMAL0;
    float3 worldNorm : NORMAL1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    
    
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv)));
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    float3 offset = vertexDistortion(modelPos.xyz, modelNorm);
    modelPos += float4(offset, 1.0f);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    float4 worldNorm = mul(float4(modelNorm, 1.0f), modelToWorld);
    
    //output.outPos = mul(worldPos, worldToClip);
    output.modelPos = modelPos.xyz;
    output.worldPos = worldPos.xyz;
    output.outCol = input.color;
    output.modelNorm = modelNorm;
    output.worldNorm = worldNorm;
    return output;
}