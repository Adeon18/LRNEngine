#include "GPUParticleStructs.hlsli"

#include "../globals.hlsli"
#include "../utility.hlsli"


cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 gResolution;
    float4 gCameraPosition;
    float4 gTimeData; // xy - time, zw - dt
};


Texture2D g_normalsBuffer : TEXTURE : register(t1);
Texture2D g_depthBuffer : TEXTURE : register(t0);


RWStructuredBuffer<GPUStructuredParticle> g_particleBuffer : register(u5);
RWBuffer<int> g_rangeBuffer : register(u6);


float3 PSPositionFromDepth(float2 vTexCoord)
{
    // Get the depth value for this pixel
    float z = g_depthBuffer.SampleLevel(g_pointWrap, float2(vTexCoord.x, vTexCoord.y), 0).r;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(vProjectedPos, worldToClipInv);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}



[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint particleIndex = dispatchThreadId.x;
    if (particleIndex >= MAX_PARTICLES)
        return;

    GPUStructuredParticle particle = g_particleBuffer[particleIndex];
    if (particle.lifeTime <= 0)
        return;
    
    float dt = gTimeData.z;

    float3 newPos = particle.centerPosition + particle.velocity * dt;
    
    float4 newPosInClip = mul(float4(newPos, 1.0f), worldToClip);
    float3 newPosDivW = newPosInClip.xyz / newPosInClip.w;
    float2 sampleCoords;
    sampleCoords.x = newPosDivW.x * 0.5 + 0.5;
    sampleCoords.y = newPosDivW.y * -0.5 + 0.5;
    
    float depthFromBuffer = g_depthBuffer.SampleLevel(g_pointWrap, sampleCoords, 0).r;
    if (depthFromBuffer > newPosDivW.z)
    {
        particle.velocity = reflect(particle.velocity, unpackOctahedron(g_normalsBuffer.SampleLevel(g_pointWrap, sampleCoords, 0).rg)) / 1.5f;
    }
    else
    {
        particle.velocity += float3(0, -9.8f, 0) * dt;
    }
    particle.colorAndAlpha.a = clamp((particle.lifeTime - 0.3f) / 0.5f, 0, 1);
    particle.centerPosition += particle.velocity * dt;
    
    particle.lifeTime -= dt;
    if (particle.lifeTime <= 0)
    {
        InterlockedAdd(g_rangeBuffer[2], 1);
    }

    // Assign the updated particle back to the output buffer
    g_particleBuffer[particleIndex] = particle;

}