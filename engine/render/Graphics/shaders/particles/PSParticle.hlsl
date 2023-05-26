#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float spawnTime : SPAWNTIME;
};

cbuffer perFrame : register(b0)
{
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

Texture2D g_textureDBF : TEXTURE : register(t0);
Texture2D g_textureMVEA : TEXTURE : register(t1);
Texture2D g_textureRLU : TEXTURE : register(t2);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float atlasTexWidth = 1.0f / atlasFrameCount.x;
    float atlasTexHeight = 1.0f / atlasFrameCount.z;
    
    float totalLifetime = atlasFrameCount.x * atlasFrameCount.z / animationSpeedFPS.x;
    
    float lived = iTime - inp.spawnTime;
    
    int textureIdx = lived * animationSpeedFPS.x;
    
    float2 uvOffset = float2(int(textureIdx % atlasFrameCount.x) * atlasTexWidth, int(textureIdx / atlasFrameCount.x) * atlasTexHeight);
    
    float2 finalUV = uvOffset + float2(inp.uv.x * atlasTexWidth, inp.uv.y * atlasTexWidth);
    
    float4 texCol = g_textureMVEA.Sample(g_linearWrap, finalUV);
    
    return float4(inp.color.rgb * texCol.z, inp.color.a * texCol.a);
}