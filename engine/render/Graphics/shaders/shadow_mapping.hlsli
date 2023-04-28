#ifndef SHADOW_MAPPING_HLSLI
#define SHADOW_MAPPING_HLSLI

#include "lighting_cook_torrance.hlsli"

cbuffer shadowMapToLightMatrices : register(b4)
{
    float4x4 dirLightViewProj[MAX_DIRLIGHT_COUNT];
    float4x4 spotLightViewProj;
}

float checkIfInShadow(float3 worldFragPos, float4x4 lightWorldProj, const Texture2D<float> shadowMap)
{
    float4 lightFragPos = mul(float4(worldFragPos, 1.0f), lightWorldProj);
    
    float3 projCoords = lightFragPos.xyz / lightFragPos.w;
    
    if (projCoords.x < -1.0f || projCoords.x > 1.0f ||
        projCoords.y < -1.0f || projCoords.y > 1.0f ||
        projCoords.z < 0.0f || projCoords.z > 1.0f)
        return 0.0;
    
    float2 sampleCoords;
    sampleCoords.x = projCoords.x / 2 + 0.5;
    sampleCoords.y = projCoords.y / -2 + 0.5;
    
    float currentDepth = projCoords.z;
    
    float closestDepth = shadowMap.Sample(g_linearWrap, sampleCoords);
    
    return currentDepth < closestDepth ? 1.0 : 0.0;
}

#endif