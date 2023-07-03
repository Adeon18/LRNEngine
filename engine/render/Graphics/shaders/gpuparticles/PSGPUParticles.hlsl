#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : WORLD_POS;
    float3 norm : NORM;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 screenUV : TEXCOORD2;
    float spawnTime : SPAWNTIME;
};

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
    float iTime;
};

cbuffer perFrame2 : register(b1)
{
    int4 atlasFrameCount;
    int4 animationSpeedFPS;
};

Texture2D g_particleTex : TEXTURE : register(t0);

Texture2D g_depthTexture : TEXTURE : register(t4);


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float4 color = g_particleTex.Sample(g_linearWrap, inp.uv);
    
    
    return float4(color.rgb, color.a * inp.color.a);
}