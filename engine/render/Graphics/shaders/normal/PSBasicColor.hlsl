#include "../globals.hlsli"
#include "../lighting.hlsli"

#include "BasicColorStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMaterialc : register(b1)
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
#if DEBUG == 1
    return float4(inp.modelNorm / 2.0f + 0.5f, 1.0f);
#else
    
    Material placeHolderMaterial = {
        float3(0.05f, 0.05f, 0.05f),
        float3(0.8f, 0.8f, 0.8f),
        float3(1.0f, 1.0f, 1.0f),
        32.0f
    };
    
    float3 camDir = normalize(iCameraPosition.xyz - inp.worldPos);
    
#if MODE == 0
    float3 colFromTex = g_texture0.Sample(g_pointWrap, inp.outTexCoord).xyz;
#elif MODE == 1
    float3 colFromTex = g_textureDiffuse.Sample(g_linearWrap, inp.outTexCoord).xyz;
#elif MODE == 2
    float3 colFromTex = g_texture0.Sample(g_anisotropicWrap, inp.outTexCoord).xyz;
#endif
    
    float3 fragNorm = (isNormalMapBound) ? getNormalFromTexture(inp.outTexCoord, inp.TBN) : inp.worldNorm;
    
    float3 outCol = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        outCol += calculateDirectionalLight(directLights[i], placeHolderMaterial, fragNorm, camDir, colFromTex);
    }
    
    
    for (int i = 0; i < pointLightCount.x; ++i)
    {
        outCol += calculatePointLight(pointLights[i], placeHolderMaterial, fragNorm, inp.worldPos, camDir, colFromTex);
    }
    
    outCol += calculateSpotLight(spotLight, placeHolderMaterial, fragNorm, inp.worldPos, camDir, colFromTex);
    
    return float4(outCol, 1.0f);
#endif
}