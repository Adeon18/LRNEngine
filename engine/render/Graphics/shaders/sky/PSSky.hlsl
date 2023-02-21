#include "globals.hlsli"

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

TextureCube textureCube;

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return textureCube.Sample(g_linearWrap, inp.modelPos);
}