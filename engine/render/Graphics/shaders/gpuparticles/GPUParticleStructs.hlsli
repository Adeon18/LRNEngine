struct GPUStructuredParticle
{
    float4 colorAndAlpha;
    float3 centerPosition;
    float3 velocity;
    float2 size;
    float spawnAtTime;
    float lifeTime;
};

static const int MAX_PARTICLES = 4096;