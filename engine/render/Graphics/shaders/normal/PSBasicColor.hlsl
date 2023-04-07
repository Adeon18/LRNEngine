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
    float4 defaultMetallic;
    float4 defaultRoughness;
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
    float metallic = (isMetallicBound) ? g_textureMetallic.Sample(g_pointWrap, inp.outTexCoord).x : DEFAULT_METALLIC;
    float roughness = (isRoughnessBound) ? g_textureRoughness.Sample(g_pointWrap, inp.outTexCoord).x : DEFAULT_ROUGHNESS;
#elif MODE == 1
    float3 albedo = g_textureDiffuse.Sample(g_linearWrap, inp.outTexCoord).xyz;
    float metallic = (isMetallicBound) ? g_textureMetallic.Sample(g_linearWrap, inp.outTexCoord).x : defaultMetallic.x;
    float roughness = (isRoughnessBound) ? g_textureRoughness.Sample(g_linearWrap, inp.outTexCoord).x : defaultRoughness.x;
#elif MODE == 2
    float3 albedo = g_texture0.Sample(g_anisotropicWrap, inp.outTexCoord).xyz;
    float metallic = (isMetallicBound) ? g_textureMetallic.Sample(g_anisotropicWrap, inp.outTexCoord).x : DEFAULT_METALLIC;
    float roughness = (isRoughnessBound) ? g_textureRoughness.Sample(g_anisotropicWrap, inp.outTexCoord).x : DEFAULT_ROUGHNESS;
#endif
    // Square the rouhnhdfhehess
    roughness = pow(roughness, 2);
    float3 viewDir = normalize(iCameraPosition.xyz - inp.worldPos);
    float3 fragNorm = (isNormalMapBound) ? getNormalFromTexture(inp.outTexCoord, inp.TBN) : inp.worldNorm;
    
    float3 outL0 = float3(0.0f, 0.0f, 0.0f);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        outL0 += calculateDirectionalLight(directLights[i], fragNorm, viewDir, albedo, F0, metallic, roughness);
    }

    for (int i = 0; i < pointLightCount.x; ++i)
    {
        outL0 += calculatePointLight(pointLights[i], fragNorm, inp.worldPos, viewDir, albedo, F0, metallic, roughness);
    }
    
    outL0 += calculateSpotLight(spotLight, fragNorm, inp.worldPos, viewDir, albedo, F0, metallic, roughness);
    
    
    return float4(outL0, 1.0f);
}