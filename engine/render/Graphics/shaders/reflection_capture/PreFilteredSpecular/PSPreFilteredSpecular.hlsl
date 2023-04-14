#include "../../globals.hlsli"
#include "../sample_util.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

cbuffer RoughnessBuffer : register(b0)
{
    float roughness;
}

TextureCube inputTextureMap : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return float4(inputTextureMap.Sample(g_linearWrap, inp.modelPos).rgb * (roughness), 1.0f);
}