#ifndef SHADOW_MAPPING_HLSLI
#define SHADOW_MAPPING_HLSLI

#include "lighting_cook_torrance.hlsli"

cbuffer shadowMapToLightMatrices : register(b4)
{
    float4x4 dirLightViewProj[MAX_DIRLIGHT_COUNT];
    float4x4 spotLightViewProj;
    float4 texelSizeClipSpaceDirectionalMap;
    float4 texelWorldSpaceSizeDirectionalMap;
}

float getSimpleLightAngleBias(float3 surfaceNorm, float3 lightDir, float maxVal, float minVal)
{
    return max(maxVal * (1.0f - dot(lightDir, surfaceNorm)), minVal);
}

//! Simple PCF that just samples 9 texels instead of 1
float simplePCF9Dir(const Texture2D<float> shadowMap, float currentDepth, float2 centerSampleCoords)
{
    float shadow = 0.0f;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = shadowMap.Sample(g_pointWrap, centerSampleCoords + float2(x, y) * texelSizeClipSpaceDirectionalMap.x);
            shadow += currentDepth < closestDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0f;
    
    return shadow;
}

//! THIS IS TAKEN FROM THE INTERNET
//! Got it from here: https://sudonull.com/post/97826-Native-implementation-of-OmniDirectional-shadows-in-DirectX11
float vectorToDepth(float3 vec, float n, float f)
{
    float3 AbsVec = abs(vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
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
    
    float bias = getSimpleLightAngleBias(macNorm, toLightDir, 0.0005f, 0.00005f);
    
    return simplePCF9Dir(shadowMap, currentDepth + bias, sampleCoords);
}

float checkIfInPointShadow(float3 worldFragPos, float3 lightPos, const TextureCube<float> shadowCubeMap, float3 macNorm)
{
    float3 lightDir = worldFragPos - lightPos;
 
    float closestDepth = shadowCubeMap.Sample(g_linearWrap, lightDir).r;
    
    float currentDepth = vectorToDepth(lightDir, 1000.0f, 0.1f);
    
    float bias = getSimpleLightAngleBias(macNorm, normalize(lightPos - worldFragPos), 0.0005f, 0.00005f);

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