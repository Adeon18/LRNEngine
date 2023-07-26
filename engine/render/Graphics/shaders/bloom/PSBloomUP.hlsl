#include "../globals.hlsli"


struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

Texture2D g_texture0 : TEXTURE : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float3 fragCol = g_texture0.Sample(g_pointWrap, inp.texCoords);
    
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}