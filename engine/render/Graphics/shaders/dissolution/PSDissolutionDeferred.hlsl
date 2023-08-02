#include "../globals.hlsli"
#include "../utility.hlsli"

#include "DissolutionStructs.hlsli"


cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
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

Texture2D g_noiseDissolution : TEXTURE : register(t5);


float3 getNormalFromTexture(float2 texCoords, float3x3 TBN)
{
    float3 normFromTex = g_textureNormalMap.Sample(g_linearWrap, texCoords).xyz;
    normFromTex = normFromTex * 2.0f - 1.0f;
    normFromTex = normalize(mul(normFromTex, TBN));
    
    return normFromTex;
}

#define DEBUG 0
#define MODE 1

static const float DISSOLUTION_BORDER_WIDTH = 0.15f;

static const float3 DISSOLUTION_COLOR = float3(1.0f, 0.5f, 0.0f);

PS_OUTPUT_DEFERRED main(PS_INPUT inp) : SV_TARGET
{
    //! Calculate the dissolution effect and discard pixels with no opacity
    float noise = g_noiseDissolution.Sample(g_linearWrap, inp.outTexCoord).x;
    float timeNormalized = (iTime - inp.outTime.x) / inp.outTime.z;
    float a = noise - 1 + timeNormalized + DISSOLUTION_BORDER_WIDTH;
    float finalA = (a < 0) ? 0 : 1;
    float dissolutionIntensity = max(0, DISSOLUTION_BORDER_WIDTH - abs(a)) / DISSOLUTION_BORDER_WIDTH;
    if (finalA == 0)
    {
        discard;
    }
    
    PS_OUTPUT_DEFERRED output;

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
    
    float2 macNormPacked = packOctahedron(inp.worldNorm);
    float2 micNormPacked = packOctahedron(micNorm);
    
    output.albedo = float4(albedo, 1.0f);
    output.normals = float4(macNormPacked, micNormPacked);
    output.roughMet = float2(roughness, metallic);
    output.emission = float4(dissolutionIntensity * DISSOLUTION_COLOR, 1.0f);
    output.objectIDs = 0;
    
    return output;
}