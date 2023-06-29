#include "GPUParticleStructs.hlsli"



RWStructuredBuffer<GPUStructuredParticle> g_particleBuffer : register(u5);
RWBuffer<int> g_rangeBuffer : register(u6);


static const int MAX_PARTICLES = 512;


[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint particleIndex = dispatchThreadId.x;
    if (particleIndex >= MAX_PARTICLES)
        return;

    GPUStructuredParticle particle = g_particleBuffer[particleIndex];
    if (particle.lifeTime <= 0)
        return;

    particle.centerPosition += particle.velocity * 0.006f;
    particle.velocity += float3(0, -9.8, 0) * 0.006f;
    particle.lifeTime -= 0.006f;
    if (particle.lifeTime <= 0)
    {
        InterlockedAdd(g_rangeBuffer[2], 1);
    }

    // Assign the updated particle back to the output buffer
    g_particleBuffer[particleIndex] = particle;

}