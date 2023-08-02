#include "../../globals.hlsli"
#include "../sample_util.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

cbuffer RoughnessBuffer : register(b0)
{
    float roughness;
}

TextureCube inputTextureMap : register(t0);

static const int SAMPLE_COUNT = 16384;

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float roughnessSquared = roughness * roughness;
    
    float3x3 hemRotationMatrix = basisFromDir(inp.modelPos);
    
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float2 randomNoise = randomHammersley(i, SAMPLE_COUNT);
        
        float3 halfVector = randomGGX(randomNoise, roughnessSquared);
        
        float3 rotatedH = mul(halfVector, hemRotationMatrix);
        
        float3 view = inp.modelPos;
        
        float3 sampleDir = 2.0 * dot(view, rotatedH) * rotatedH - view;
        
        float3 E_k = inputTextureMap.Sample(g_linearWrap, sampleDir);
        
        float NdotL = dot(inp.modelPos, sampleDir);
        
        if (NdotL > 0.0)
        {
            irradiance += E_k;
        }
    }
    
    irradiance /= SAMPLE_COUNT;
    
    return float4(irradiance, 1.0f);
}