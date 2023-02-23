#include "../globals.hlsli"

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};


struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float4 outCol : COLOR;
    float3 modelNorm : NORM;
    float2 outTexCoord : TEXCOORD;
};

Texture2D g_texture0 : TEXTURE : register(t0);

#define DEBUG 0
#define MODE 1

float4 main(PS_INPUT inp) : SV_TARGET
{
#if DEBUG == 1
    return float4(inp.modelNorm / 2.0f + 0.5f, 1.0f);
#else
    
#if MODE == 0
    return g_texture0.Sample(g_pointWrap, inp.outTexCoord);
#elif MODE == 1
    return g_texture0.Sample(g_linearWrap, inp.outTexCoord);
#elif MODE == 2
    return g_texture0.Sample(g_anisotropicWrap, inp.outTexCoord);
#endif
    
#endif
}