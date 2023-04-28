#ifndef SHADOW_MAPPING_HLSLI
#define SHADOW_MAPPING_HLSLI

#include "lighting_cook_torrance.hlsli"

cbuffer shadowMapToLightMatrices : register(b4)
{
    float4x4 dirLightViewProj[MAX_DIRLIGHT_COUNT];
    float4x4 spotLightViewProj;
}

float getSimpleLightAngleBias(float3 surfaceNorm, float3 lightDir, float maxVal, float minVal)
{
    return max(maxVal * (1.0f - dot(lightDir, surfaceNorm)), minVal);
}

float checkIfInDirectionalShadow(float3 worldFragPos, float4x4 lightWorldProj, const Texture2D<float> shadowMap, float3 toLightDir, float3 macNorm)
{
    //! TODO: This might be sped up
    if (dot(macNorm, toLightDir) < MIN_LIGHT_INTENCITY)
    {
        return 0.0f;
    }
    
    float4 lightFragPos = mul(float4(worldFragPos, 1.0f), lightWorldProj);
    float3 projCoords = lightFragPos.xyz / lightFragPos.w;
    
    //! This ignores the projection border
    if (projCoords.x < -1.0f || projCoords.x > 1.0f ||
        projCoords.y < -1.0f || projCoords.y > 1.0f ||
        projCoords.z < 0.0f || projCoords.z > 1.0f)
        return 0.0;
    
    //! TODO: understand why the hell
    float2 sampleCoords;
    sampleCoords.x = projCoords.x / 2 + 0.5;
    sampleCoords.y = projCoords.y / -2 + 0.5;
    
    float currentDepth = projCoords.z;
    float closestDepth = shadowMap.Sample(g_pointWrap, sampleCoords);
    
    float bias = getSimpleLightAngleBias(macNorm, toLightDir, 0.0005f, 0.00005f);
    
    return currentDepth + bias < closestDepth ? 1.0 : 0.0;
}

float checkIfInSpotShadow(float3 worldFragPos, float4x4 lightWorldProj, const Texture2D<float> shadowMap, float3 toLightDir, float3 macNorm)
{
    //! TODO: This might be sped up
    if (dot(macNorm, toLightDir) < MIN_LIGHT_INTENCITY)
    {
        return 0.0f;
    }
    
    float4 lightFragPos = mul(float4(worldFragPos, 1.0f), lightWorldProj);
    float3 projCoords = lightFragPos.xyz / lightFragPos.w;
    
    //! This ignores the projection border
    if (projCoords.x < -1.0f || projCoords.x > 1.0f ||
        projCoords.y < -1.0f || projCoords.y > 1.0f ||
        projCoords.z < 0.0f || projCoords.z > 1.0f)
        return 0.0;
    
    //! TODO: understand why the hell
    float2 sampleCoords;
    sampleCoords.x = projCoords.x / 2 + 0.5;
    sampleCoords.y = projCoords.y / -2 + 0.5;
    
    float currentDepth = projCoords.z;
    float closestDepth = shadowMap.Sample(g_pointWrap, sampleCoords);
    
    float bias = getSimpleLightAngleBias(macNorm, toLightDir, 0.05f, 0.005f);
    
    return currentDepth + bias < closestDepth ? 1.0 : 0.0;
}

#endif