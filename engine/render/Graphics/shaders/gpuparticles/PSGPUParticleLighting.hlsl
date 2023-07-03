#include "../globals.hlsli"
#include "../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : WORLD_POS;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float spawnTime : SPAWNTIME;
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

cbuffer perFrame2 : register(b1)
{
    int4 atlasFrameCount;
    int4 animationSpeedFPS;
};

Texture2D g_particleTex : TEXTURE : register(t0);
Texture2D g_albedoDef : TEXTURE : register(t1);
Texture2D g_normDef : TEXTURE : register(t2);
Texture2D g_roughMetDef : TEXTURE : register(t3);
Texture2D g_depthTexture : TEXTURE : register(t4);

float3 PSPositionFromDepth(float2 vTexCoord)
{
    // Get the depth value for this pixel
    float z = g_depthTexture.Sample(g_pointWrap, float2(vTexCoord.x, vTexCoord.y)).r;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(vProjectedPos, worldToClipInv);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}


float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float2 sampleCoords = inp.clipPos.xy / iResolution.xy;
    float3 worldPos = PSPositionFromDepth(sampleCoords);

    if (length(worldPos - inp.worldPos.xyz) > 0.3f)
        discard;
    
    float4 color = g_particleTex.Sample(g_linearWrap, inp.uv);
    // So the quads are not visible
    if (color.a < 0.7f)
        discard;
    
    float3 albedo = g_albedoDef.Sample(g_pointWrap, sampleCoords).xyz;
    float2 roughnessMetallic = g_roughMetDef.Sample(g_pointWrap, sampleCoords).xy;
    float metallic = roughnessMetallic.y;
    float roughness = roughnessMetallic.x;
    
    float4 normalsPacked = g_normDef.Sample(g_pointWrap, sampleCoords);
    
    float3 macNorm = unpackOctahedron(normalsPacked.xy);
    float3 micNorm = unpackOctahedron(normalsPacked.zw);
            
    float3 viewDir = normalize(iCameraPosition.xyz - worldPos);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);
    
    float3 L0 = getLambertDiffuse(albedo, micNorm, normalize(inp.worldPos.xyz - worldPos), F0, metallic, 10.0f);
    
    // Interpolate lighting
    return float4(L0, 1.0f * inp.color.a);
}