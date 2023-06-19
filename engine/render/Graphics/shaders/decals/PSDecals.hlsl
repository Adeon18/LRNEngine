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

PS_OUTPUT_DEFERRED main(PS_INPUT inp) : SV_TARGET
{
    PS_OUTPUT_DEFERRED output;
 
    output.albedo = float4(0, 0, 0, 0.0f);
    output.normals = float4(0, 0, 0, 0);
    output.roughMet = float2(0, 0);
    output.emission = float4(1.0f, 0.0f, 0.0f, 1.0f);
    output.objectIDs = 0;
    
    return output;
}