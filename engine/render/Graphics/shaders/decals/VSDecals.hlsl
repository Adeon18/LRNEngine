#include "DecalStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 decalToWorld = float4x4(input.decalToWorld0, input.decalToWorld1, input.decalToWorld2, input.decalToWorld3);
    float4x4 decalToWorldInv = float4x4(input.decalToWorldInv0, input.decalToWorldInv1, input.decalToWorldInv2, input.decalToWorldInv3);
    
    float4 worldPos = mul(float4(input.inPos, 1.0f), decalToWorld);

    output.outPos = mul(worldPos, worldToClip);
    output.worldToDecal = decalToWorldInv;
    output.decalToWorld = decalToWorld;
    output.uv = input.inTC;

    return output;
}