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

Texture2D g_texture0 : TEXTURE : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float2 texSize = 1.0f / iResolution.xy;
    
    float xS = texSize.x;
    float yS = texSize.y;
    
    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    float3 a = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - 2 * xS, inp.texCoords.y + 2 * yS)).rgb;
    float3 b = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y + 2 * yS)).rgb;
    float3 c = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + 2 * xS, inp.texCoords.y + 2 * yS)).rgb;

    float3 d = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - 2 * xS, inp.texCoords.y)).rgb;
    float3 e = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y)).rgb;
    float3 f = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + 2 * xS, inp.texCoords.y)).rgb;

    float3 g = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - 2 * xS, inp.texCoords.y - 2 * yS)).rgb;
    float3 h = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x, inp.texCoords.y - 2 * yS)).rgb;
    float3 i = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + 2 * xS, inp.texCoords.y - 2 * yS)).rgb;

    float3 j = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - xS, inp.texCoords.y + yS)).rgb;
    float3 k = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + xS, inp.texCoords.y + yS)).rgb;
    float3 l = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x - xS, inp.texCoords.y - yS)).rgb;
    float3 m = g_texture0.Sample(g_pointWrap, float2(inp.texCoords.x + xS, inp.texCoords.y - yS)).rgb;
    
    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    
    float3 downSample = e * 0.125;
    downSample += (a + c + g + i) * 0.03125;
    downSample += (b + d + f + h) * 0.0625;
    downSample += (j + k + l + m) * 0.125;
    
    return float4(downSample, 1.0f);
}