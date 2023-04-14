#include "../sample_util.hlsli"
#include "../../lighting_cook_torrance.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

static const int SAMPLE_COUNT = 1024;

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    float3 normal = float3(0.0f, 0.0f, 1.0f);
    
    float roughness = inp.texCoords.y;
    float NdotV = inp.texCoords.x;
    
    float specR = 0.0f;
    float specG = 0.0f;
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float2 randomNoise = randomHammersley(i, SAMPLE_COUNT);
        
        float3 halfVector = randomGGX(randomNoise, roughness * roughness);
        
        float3 view = float3(sqrt(1.0 - NdotV * NdotV), 0.0f, NdotV);
        
        float3 sampleDir = 2.0 * dot(view, halfVector) * halfVector - view;
        
        float NdotL = dot(sampleDir, normal);
        float NdotH = dot(halfVector, normal);
        float HdotV = dot(halfVector, view);
        
        if (NdotL > 0)
        {
            float G = smith(roughness, NdotV, NdotL);
        
            float K_specR = (G * (1 - pow((1 - HdotV), 5)) * HdotV) / (NdotV * NdotH);
            float K_specG = (G * pow((1 - HdotV), 5) * HdotV) / (NdotV * NdotH);
            
            specR += K_specR;
            specG += K_specG;
        }
    }
    
    specR /= SAMPLE_COUNT;
    specG /= SAMPLE_COUNT;
    
    return float4(specR, specG, 0.0f, 1.0f);
}