#include "../globals.hlsli"

/// Shader taken from https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom and converted to HLSL

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
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

static const float RADIUS = 0.005f;

Texture2D g_texture0 : TEXTURE : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float2 texSize = 1.0f / iResolution.xy;
    
    float xS = RADIUS;
    float yS = RADIUS;
    
    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - xS, inp.texCoords.y + yS)).rgb;
    float3 b = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y + yS)).rgb;
    float3 c = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + xS, inp.texCoords.y + yS)).rgb;

    float3 d = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - xS, inp.texCoords.y)).rgb;
    float3 e = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y)).rgb;
    float3 f = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + xS, inp.texCoords.y)).rgb;

    float3 g = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - xS, inp.texCoords.y - yS)).rgb;
    float3 h = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y - yS)).rgb;
    float3 i = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + xS, inp.texCoords.y - yS)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    float3 upSample = e * 4.0;
    upSample += (b + d + f + h) * 2.0;
    upSample += (a + c + g + i);
    upSample *= 1.0 / 16.0;
    
    return float4(upSample, 1.0f);
}