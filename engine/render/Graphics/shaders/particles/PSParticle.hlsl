#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D g_textureDBF : TEXTURE : register(t0);
Texture2D g_textureMVEA : TEXTURE : register(t1);
Texture2D g_textureRLU : TEXTURE : register(t2);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return float4(g_textureDBF.Sample(g_linearWrap, inp.uv).rgb, inp.color.a);
}