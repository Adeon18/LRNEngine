#include "IncinerationStructs.hlsli"
#include "../gpuparticles/GPUParticleStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perMesh : register(b1)
{
    float4x4 meshToModel;
    float4x4 meshToModelInv;
};

RWStructuredBuffer<GPUStructuredParticle> g_particleBuffer : register(u5);
RWBuffer<int> g_rangeBuffer : register(u6);

static const float PARTICLE_INITIAL_SPEED = 6.0f;
static const float PARTICLE_LIFETIME = 4.0f;
static const int VERTICES_PER_PARTICLE = 50;


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 modelToWorld = float4x4(input.modelToWorld0, input.modelToWorld1, input.modelToWorld2, input.modelToWorld3);
    float4x4 modelToWorldInv = float4x4(input.modelToWorldInv0, input.modelToWorldInv1, input.modelToWorldInv2, input.modelToWorldInv3);
    
    // Normal is transformed via transpose of the inverse
    float3 modelNorm = normalize(mul(float4(input.inNorm, 0.0f), transpose(meshToModelInv))).xyz;
    float3 modelTan = normalize(mul(float4(input.inTangent, 0.0f), transpose(meshToModelInv))).xyz;
    float3 modelBiTan = normalize(mul(float4(input.inBiTangent, 0.0f), transpose(meshToModelInv))).xyz;
    float4 modelPos = mul(float4(input.inPos, 1.0f), meshToModel);
    
    float4 worldPos = mul(modelPos, modelToWorld);
    // The world inv is transposed at entering the shader
    float3 worldNorm = normalize(mul(float4(modelNorm, 0.0f), modelToWorldInv)).xyz;
    float3 worldTan = normalize(mul(float4(modelTan, 0.0f), modelToWorldInv)).xyz;
    float3 worldBiTan = normalize(mul(float4(modelBiTan, 0.0f), modelToWorldInv)).xyz;
    
    if (input.vertexId % VERTICES_PER_PARTICLE == 0)
    {
        if (length(worldPos.xyz - input.hitPosAndMaxRadius.xyz) < input.prevCurRad.y && !((length(worldPos.xyz - input.hitPosAndMaxRadius.xyz) < input.prevCurRad.x)))
        {
            GPUStructuredParticle b;
            b.colorAndAlpha = float4(1, 1, 0, 1);
            b.centerPosition = worldPos.xyz + 0.05f * worldNorm;
            b.velocity = worldNorm * PARTICLE_INITIAL_SPEED;
            b.size = float2(0.25, 0.25);
            b.spawnAtTime = iTime;
            b.lifeTime = PARTICLE_LIFETIME;

            if (g_rangeBuffer[0] < MAX_PARTICLES)
            {
                int prevVal;
                int prevOffset;
                InterlockedAdd(g_rangeBuffer[0], 1, prevVal);
                InterlockedAdd(g_rangeBuffer[1], 0, prevOffset);
                g_particleBuffer[(prevOffset + prevVal) % MAX_PARTICLES] = b;
            }
        }
    }
    
    output.outPos = mul(worldPos, worldToClip);
    output.worldPos = worldPos.xyz;
    output.outTime = input.time;
    output.worldNorm = worldNorm;
    output.outTexCoord = input.inTC;
    output.TBN = float3x3(worldTan, worldBiTan, worldNorm);
    output.hitPosAndMaxRad = input.hitPosAndMaxRadius;
    output.currentRad = input.prevCurRad.y;
    return output;
}