#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

Texture2D g_texture0 : TEXTURE : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return g_texture0.Sample(g_linearWrap, inp.texCoords);
}