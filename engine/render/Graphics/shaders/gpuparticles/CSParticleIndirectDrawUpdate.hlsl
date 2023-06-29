#include "GPUParticleStructs.hlsli"



RWStructuredBuffer<GPUStructuredParticle> g_particleBuffer : register(u5);
RWBuffer<int> g_rangeBuffer : register(u6);

static const int MAX_PARTICLES = 512;

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint particleIndex = dispatchThreadId.x;
    if (particleIndex > 0)
        return;

    g_rangeBuffer[1] = (g_rangeBuffer[1] + g_rangeBuffer[2]) % MAX_PARTICLES;
    g_rangeBuffer[0] = g_rangeBuffer[0] - g_rangeBuffer[2];
    g_rangeBuffer[2] = 0;

    g_rangeBuffer[3] = 4;   // IndexCountPerInstance
    g_rangeBuffer[4] = g_rangeBuffer[0];   // InstanceCount
    g_rangeBuffer[5] = 0;   // StartIndexLocation
    g_rangeBuffer[6] = 0;   // BaseVertexLocation
    g_rangeBuffer[7] = 0;   // StartInstanceLocation
}