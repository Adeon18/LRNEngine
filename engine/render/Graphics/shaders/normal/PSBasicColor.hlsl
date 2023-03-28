#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

#include "BasicColorStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMaterial : register(b1)
{
    bool isDiffuseBound;
    bool isNormalMapBound;
    bool isRoughnessBound;
    bool isMetallicBound;
}

Texture2D g_textureDiffuse : TEXTURE : register(t0);
Texture2D g_textureNormalMap : TEXTURE : register(t1);
Texture2D g_textureRoughness : TEXTURE : register(t2);
Texture2D g_textureMetallic : TEXTURE : register(t3);

float3 getNormalFromTexture(float2 texCoords, float3x3 TBN)
{
    float3 normFromTex = g_textureNormalMap.Sample(g_linearWrap, texCoords).xyz;
    normFromTex = normFromTex * 2.0f - 1.0f;
    normFromTex = normalize(mul(normFromTex, TBN));
    
    return normFromTex;
}

#define DEBUG 0
#define MODE 1

float4 main(PS_INPUT inp) : SV_TARGET
{
   
    
#if MODE == 0
    float3 albedo = g_texture0.Sample(g_pointWrap, inp.outTexCoord).xyz;
#elif MODE == 1
    float3 albedo = g_textureDiffuse.Sample(g_linearWrap, inp.outTexCoord).xyz;
    float metallic = (isMetallicBound) ? g_textureMetallic.Sample(g_linearWrap, inp.outTexCoord).x: 0.5f;
    float roughness = (isRoughnessBound) ? g_textureRoughness.Sample(g_linearWrap, inp.outTexCoord).x: 0.5f;
#elif MODE == 2
    float3 albedo = g_texture0.Sample(g_anisotropicWrap, inp.outTexCoord).xyz;
#endif
    
    float3 viewDir = normalize(iCameraPosition.xyz - inp.worldPos);
    float3 fragNorm = (isNormalMapBound) ? getNormalFromTexture(inp.outTexCoord, inp.TBN) : inp.worldNorm;
    
    float3 outL0 = float3(0.0f, 0.0f, 0.0f);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
    
    for (int i = 0; i < pointLightCount.x; ++i)
    {
        float3 lightDir = normalize(pointLights[i].position.xyz - inp.worldPos);
        float3 halfVector = normalize(viewDir + lightDir);
        
        float3 NdotL = max(dot(fragNorm, lightDir), MIN_LIGHT_INTENCITY);
        
        outL0 += pointLights[i].color.xyz * pointLights[i].intensity.xyz *
        (getLambertDiffuse(albedo, fragNorm, lightDir, F0, metallic, 1.0f) * NdotL +
            getCookTorrenceSpecular(fragNorm, halfVector, viewDir, lightDir, 1.0f, roughness, F0));
    }
    
    
    return float4(outL0, 1.0f);
}