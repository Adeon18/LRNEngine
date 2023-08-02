#pragma once

#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

// lighting_cook_torrance has buffers 2 and 3

Texture2D g_albedoDef : TEXTURE : register(t0);
Texture2D g_normDef : TEXTURE : register(t1);
Texture2D g_roughMetDef : TEXTURE : register(t2);
Texture2D g_emissionDef : TEXTURE : register(t3);
Texture2D g_objIDDef : TEXTURE : register(t4);


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return float4(g_emissionDef.Sample(g_pointWrap, inp.texCoords).xyz, 1.0f);
}