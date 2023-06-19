#include "../globals.hlsli"
#include "../utility.hlsli"

#include "DecalStructs.hlsli"


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

Texture2D g_depthBuffer : TEXTURE : register(t5);

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


PS_OUTPUT_DEFERRED main(PS_INPUT inp) : SV_TARGET
{
    PS_OUTPUT_DEFERRED output;
    
    float3 worldPos = PSPositionFromDepth(inp.outPos.xy / iResolution.xy);
    
    float3 decalPos = mul(float4(worldPos, 1.0f), inp.worldToDecal);
    
    if (decalPos.x > 0.5f || decalPos.x < -0.5f ||
        decalPos.y > 0.5f || decalPos.y < -0.5f ||
        decalPos.z > 0.5f || decalPos.z < -0.5f)
    {
        discard;
    }
    
    output.albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //output.normals = float4(0, 0, 0, 0);
    output.roughMet = float2(0, 0);
    output.emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.objectIDs = 0;
    
    return output;
}