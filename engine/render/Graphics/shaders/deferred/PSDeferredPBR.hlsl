#include "../globals.hlsli"
#include "../utility.hlsli"
#include "../shadow_mapping.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float4 iPFSCubemapResolution;
    float iTime;
};

// lighting_cook_torrance has buffers 2 and 3

Texture2D g_albedoDef : TEXTURE : register(t0);
Texture2D g_normDef : TEXTURE : register(t1);
Texture2D g_roughMetDef : TEXTURE : register(t2);
Texture2D g_emissionDef : TEXTURE : register(t3);
Texture2D g_objIDDef : TEXTURE : register(t4);
Texture2D g_depthBuffer : TEXTURE : register(t5);

TextureCube g_diffuseIrradiance : register(t6);
TextureCube g_preFilteredSpecular : register(t7);
Texture2D g_BRDFIntegration : TEXTURE : register(t8);

Texture2D<float> g_spotLightShadowMap : TEXTURE : register(t10);

//! Only 1 directional light but have possibility to support more
Texture2D<float> g_directionalLightShadowMaps[MAX_DIRLIGHT_COUNT] : register(t11);
TextureCube<float> g_pointLightShadowMaps[4] : register(t12);


float3 PSPositionFromDepth(float2 vTexCoord)
{
    // Get the depth value for this pixel
    float z = g_depthBuffer.Sample(g_pointWrap, float2(vTexCoord.x, vTexCoord.y)).r;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(vProjectedPos, worldToClipInv);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}


static const int PFS_SAMPLE_COUNT = 16384;


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float3 outL0 = float3(0.0f, 0.0f, 0.0f);
    
    float3 albedo = g_albedoDef.Sample(g_pointWrap, inp.texCoords).xyz;
    float2 roughnessMetallic = g_roughMetDef.Sample(g_pointWrap, inp.texCoords).xy;
    float3 emission = g_emissionDef.Sample(g_pointWrap, inp.texCoords).xyz;
    float metallic = roughnessMetallic.y;
    float roughness = roughnessMetallic.x;
    
    float4 normalsPacked = g_normDef.Sample(g_pointWrap, inp.texCoords);
    
    float3 macNorm = unpackOctahedron(normalsPacked.xy);
    float3 micNorm = unpackOctahedron(normalsPacked.zw);
    
    float3 worldPos = PSPositionFromDepth(inp.texCoords);
        
    float3 viewDir = normalize(iCameraPosition.xyz - worldPos);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
    
    for (int i = 0; i < dirLightCount.x; ++i)
    {
        float shadow = checkIfInDirectionalShadow(worldPos, dirLightViewProj[i], g_directionalLightShadowMaps[i], -directLights[i].direction.xyz, macNorm);
        outL0 += (1 - shadow) * calculateDirectionalLight(directLights[i], micNorm, viewDir, albedo, F0, metallic, roughness);
    }

    for (int i = 0; i < pointLightCount.x; ++i)
    {
        float shadow = checkIfInPointShadowViaTransform(worldPos, pointLights[i].position.xyz, g_pointLightShadowMaps[i], macNorm, i);
        outL0 += (1 - shadow) * calculatePointLight(pointLights[i], micNorm, macNorm, worldPos, viewDir, albedo, F0, metallic, roughness);
    }
    
    //! Spotlight
    {
        float3 toLight = normalize(spotLight.position.xyz - worldPos);
        float shadow = checkIfInSpotShadow(worldPos, spotLightViewProj, g_spotLightShadowMap, toLight, macNorm);
        outL0 += (1 - shadow) * calculateSpotLight(spotLight, micNorm, worldPos, viewDir, albedo, F0, metallic, roughness);
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
    
    // Yeah, so I have black pixels in a lot of cases for some reason, this fixes it
    if (IsNaN(outL0.x))
        outL0.x = albedo.x;
    if (IsNaN(outL0.y))
        outL0.y = albedo.y;
    if (IsNaN(outL0.z))
        outL0.z = albedo.z;
    
    return float4(outL0 + emission, 1.0f);
   
}