#include "../globals.hlsli"
#include "../lighting.hlsli"

cbuffer perFrame : register(b0)
{
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};


struct PS_INPUT
{
    float4 outPos : SV_POSITION;
    float3 worldPos : POS;
    float4 outCol : COLOR;
    float3 worldNorm : NORM;
    float2 outTexCoord : TEXCOORD;
};

Texture2D g_texture0 : TEXTURE : register(t0);

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
    float3 colFromTex = g_texture0.Sample(g_linearWrap, inp.outTexCoord).xyz;
#elif MODE == 2
    float3 colFromTex = g_texture0.Sample(g_anisotropicWrap, inp.outTexCoord).xyz;
#endif
    
    float3 outCol = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        outCol += calculateDirectionalLight(directLights[i], placeHolderMaterial, inp.worldNorm, camDir, colFromTex);
    }
    
    
    for (int i = 0; i < pointLightCount.x; ++i)
    {
        outCol += calculatePointLight(pointLights[i], placeHolderMaterial, inp.worldNorm, inp.worldPos, camDir, colFromTex);
    }
    
    outCol += calculateSpotLight(spotLight, placeHolderMaterial, inp.worldNorm, inp.worldPos, camDir, colFromTex);
    
    return float4(pow(outCol, 2.2f), 1.0f);
#endif
}