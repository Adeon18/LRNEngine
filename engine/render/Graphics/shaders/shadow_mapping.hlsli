#ifndef SHADOW_MAPPING_HLSLI
#define SHADOW_MAPPING_HLSLI

#include "lighting_cook_torrance.hlsli"

SamplerComparisonState g_comparisonGEWrap : register(s3);

cbuffer shadowMapToLightMatrices : register(b4)
{
    float4x4 pointLightViewProj[MAX_POINTLIGHT_COUNT][6];
    float4x4 dirLightViewProj[MAX_DIRLIGHT_COUNT];
    float4x4 spotLightViewProj;
    float4 texelSizeClipSpaceDirectionalMap;
    float4 texelWorldSpaceSizeDirectionalMap;
}

float getSimpleLightAngleBias(float3 surfaceNorm, float3 lightDir, float maxVal, float minVal)
{
    return max(maxVal * (1.0f - dot(lightDir, surfaceNorm)), minVal);
}

//! Get the cubemap face from the direction given
int getFaceFromDir(float3 dir)
{
    float maxAbsVal = max(abs(dir.x), max(abs(dir.y), abs(dir.z)));
    
    if (maxAbsVal == dir.x)
    {
        return 0;
    } else if (maxAbsVal == -dir.x)
    {
        return 1;
    } else if (maxAbsVal == dir.y)
    {
        return 2;
    } else if (maxAbsVal == -dir.y)
    {
        return 3;
    } else if (maxAbsVal == dir.z)
    {
        return 4;
    } else
    {
        return 5;
    }
}

//! Simple PCF that just samples 9 texels instead of 1
float simplePCF9Dir(const Texture2D<float> shadowMap, float currentDepth, float2 centerSampleCoords)
{
    float shadow = 0.0f;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            shadow += shadowMap.SampleCmpLevelZero(g_comparisonGEWrap, centerSampleCoords, currentDepth, float2(x, y));
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

//! Use a transformation lifehack(without matrices)
float checkIfInPointShadow(float3 worldFragPos, float3 lightPos, const TextureCube<float> shadowCubeMap, float3 macNorm)
{
    float3 lightDir = worldFragPos - lightPos;
 
    float closestDepth = shadowCubeMap.Sample(g_linearWrap, lightDir).r;
    
    float currentDepth = vectorToDepth(lightDir, 1000.0f, 0.1f);
    
    float bias = getSimpleLightAngleBias(macNorm, normalize(lightPos - worldFragPos), 0.0005f, 0.00005f);

    return currentDepth + bias < closestDepth ? 1.0 : 0.0;
}

//! Use matrix transformations by determining the right cubemap side
float checkIfInPointShadowViaTransform(float3 worldFragPos, float3 lightPos, const TextureCube<float> shadowCubeMap, float3 macNorm, int lightIdx)
{
    float3 fragDir = worldFragPos - lightPos;
    
    int matIdx = getFaceFromDir(fragDir);
    
    float4 lightFragPos = mul(float4(worldFragPos, 1.0f), pointLightViewProj[lightIdx][matIdx]);
    float3 projCoords = lightFragPos.xyz / lightFragPos.w;
    float currentDepth = projCoords.z;
    
    float bias = getSimpleLightAngleBias(macNorm, normalize(lightPos - worldFragPos), 0.001f, 0.00005f);
    
    return shadowCubeMap.SampleCmpLevelZero(g_comparisonGEWrap, fragDir, currentDepth + bias);
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
    
    float bias = getSimpleLightAngleBias(macNorm, toLightDir, 0.005f, 0.0005f);
    
    return shadowMap.SampleCmpLevelZero(g_comparisonGEWrap, sampleCoords, currentDepth + bias);
}

#endif