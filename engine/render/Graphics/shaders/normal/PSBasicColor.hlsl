#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"
#include "../shadow_mapping.hlsli"

#include "BasicColorStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
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

// lighting_cook_torrance has buffers 2 and 3

Texture2D g_textureDiffuse : TEXTURE : register(t0);
Texture2D g_textureNormalMap : TEXTURE : register(t1);
Texture2D g_textureRoughness : TEXTURE : register(t2);
Texture2D g_textureMetallic : TEXTURE : register(t3);

TextureCube g_diffuseIrradiance : register(t6);
TextureCube g_preFilteredSpecular : register(t7);
Texture2D g_BRDFIntegration : TEXTURE : register(t8);

Texture2D<float> g_spotLightShadowMap : TEXTURE : register(t10);

Texture2D<float> g_directionalLightShadowMaps[MAX_DIRLIGHT_COUNT] : register(t11);

float3 getNormalFromTexture(float2 texCoords, float3x3 TBN)
{
    float3 normFromTex = g_textureNormalMap.Sample(g_linearWrap, texCoords).xyz;
    normFromTex = normFromTex * 2.0f - 1.0f;
    normFromTex = normalize(mul(normFromTex, TBN));
    
    return normFromTex;
}

#define DEBUG 0
#define MODE 1

static const int PFS_SAMPLE_COUNT = 16384;

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
    float3 micNorm = (isNormalMapBound) ? getNormalFromTexture(inp.outTexCoord, inp.TBN) : inp.worldNorm;
    
    float3 outL0 = float3(0.0f, 0.0f, 0.0f);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        float shadow = checkIfInDirectionalShadow(inp.worldPos, dirLightViewProj[i], g_directionalLightShadowMaps[i], -directLights[i].direction.xyz, inp.worldNorm);
        outL0 += (1 - shadow) * calculateDirectionalLight(directLights[i], micNorm, viewDir, albedo, F0, metallic, roughness);
    }

    for (int i = 0; i < pointLightCount.x; ++i)
    {
        outL0 += calculatePointLight(pointLights[i], micNorm, inp.worldNorm, inp.worldPos, viewDir, albedo, F0, metallic, roughness);
    }
    
    //! Spotlight
    {
        float3 toLight = normalize(spotLight.position.xyz - inp.worldPos);
        float shadow = checkIfInSpotShadow(inp.worldPos, spotLightViewProj, g_spotLightShadowMap, toLight, inp.worldNorm);
        outL0 += (1 - shadow) * calculateSpotLight(spotLight, micNorm, inp.worldPos, viewDir, albedo, F0, metallic, roughness);
    }

    // IBL
    if (isIBLEnabled)
    {
        float S_importance = 4 / (2 * PI * ggx(roughness, 1.0f) * PFS_SAMPLE_COUNT);
        float mipLevel = hemisphereMip(S_importance, iPFSCubemapResolution.x);
        float BIUV = float2(roughness, dot(micNorm, viewDir));
        
        float3 E_spec = g_preFilteredSpecular.SampleLevel(g_linearWrap, reflect(-viewDir, micNorm), mipLevel);
        float2 K_spec = g_BRDFIntegration.Sample(g_linearWrap, BIUV).rg;
        
        outL0 += albedo * (1 - metallic) * g_diffuseIrradiance.Sample(g_linearWrap, micNorm).rgb + E_spec * (K_spec.r * F0 + K_spec.g);
    }
    
    
    return float4(outL0, 1.0f);
}