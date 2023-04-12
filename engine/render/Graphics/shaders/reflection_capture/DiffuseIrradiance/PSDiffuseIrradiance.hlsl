#include "../../globals.hlsli"
#include "../sample_util.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

TextureCube inputTextureMap : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return inputTextureMap.Sample(g_linearWrap, inp.modelPos);
}