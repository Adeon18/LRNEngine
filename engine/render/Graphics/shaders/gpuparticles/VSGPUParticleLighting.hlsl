#include "GPUParticleStructs.hlsli"

cbuffer perFrame : register(b0)
{
    float4x4 worldToClip;
    float4x4 worldToClipInv;
    float4 iResolution;
    float4 iCameraPosition;
    float iTime;
};

cbuffer perFrame2 : register(b1)
{
    float4 cameraPosition;
};

// Frisvad with z == -1 problem avoidance
void basisFromDir(out float3 right, out float3 top, in float3 dir)
{
    float k = 1.0 / max(1.0 + dir.z, 0.00001);
    float a = dir.y * k;
    float b = dir.y * a;
    float c = -dir.x * a;
    right = float3(dir.z + b, c, -dir.x);
    top = float3(c, 1.0 - b, -dir.y);
}

// Frisvad with z == -1 problem avoidance
float3x3 basisFromDir(float3 dir)
{
    float3x3 rotation;
    rotation[2] = dir;
    basisFromDir(rotation[0], rotation[1], dir);
    return rotation;
}


struct VS_IN
{
    uint vertexId : SV_VertexID;
    uint instanceId : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : WORLD_POS;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float spawnTime : SPAWNTIME;
};

struct QuadVertex
{
    float3 pos;
    float2 uv;
};

QuadVertex getQuadVertexFromID(uint vertexId, float2 size, float3 particlePos, float3 up, float3 right)
{
    QuadVertex output;
    
    if (vertexId == 0)
    {
        output.pos = particlePos - size.x / 2.0f * right - size.y / 2.0f * up;
        output.uv = float2(0, 1);
    }
    else if (vertexId == 1)
    {
        output.pos = particlePos + size.x / 2.0f * right - size.y / 2.0f * up;
        output.uv = float2(1, 1);
    }
    else if (vertexId == 2)
    {
        output.pos = particlePos - size.x / 2.0f * right + size.y / 2.0f * up;
        output.uv = float2(0, 0);
    }
    else if (vertexId == 3)
    {
        output.pos = particlePos + size.x / 2.0f * right + size.y / 2.0f * up;
        output.uv = float2(1, 0);
    }
    return output;
}

RWStructuredBuffer<GPUStructuredParticle> g_particleBuffer : register(u5);
RWBuffer<int> g_rangeBuffer : register(u6);

VS_OUTPUT main(VS_IN input)
{
    VS_OUTPUT output;
    
    GPUStructuredParticle particle = g_particleBuffer[g_rangeBuffer[1] + input.instanceId];
    
    float3 upVector = float3(0, 1, 0.0f);
    //float3 upVector = float3(0.0f, 1.0f, 0.0f);
    float3 planeNormal = normalize(particle.centerPosition - cameraPosition.xyz);
    float3 rightVector = normalize(cross(planeNormal, upVector));
    upVector = normalize(cross(rightVector, planeNormal));
    
    QuadVertex v = getQuadVertexFromID(input.vertexId, particle.size, particle.centerPosition, upVector, rightVector);
    
    float4 worldPos = float4(v.pos, 1.0f);
    
    output.clipPos = mul(worldPos, worldToClip);
    output.worldPos = float4(particle.centerPosition, 1.0f);
    output.uv = v.uv;
    output.color = particle.colorAndAlpha;
    output.spawnTime = particle.spawnAtTime;
    // Fill each vertex separately depending on vertex ID => CLOCKVISE
    
    return output;
}