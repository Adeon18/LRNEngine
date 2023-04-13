#include "../../globals.hlsli"
#include "../sample_util.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

TextureCube inputTextureMap : register(t0);

static const int SAMPLE_COUNT = 16384;

float4 main(VS_OUTPUT inp) : SV_TARGET
{    
    float3x3 hemRotationMatrix = basisFromDir(inp.modelPos);
    
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float NdotV;
        float3 sampleDir = randomHemisphere(NdotV, i, SAMPLE_COUNT);
        
        float3 dirRotated = mul(sampleDir, hemRotationMatrix);
                
        float3 E_k = inputTextureMap.Sample(g_linearWrap, dirRotated);
        
        irradiance += (E_k * NdotV) / PI * (1 - fresnel(NdotV, float3(0.04f, 0.04f, 0.04f)));
    }
    
    irradiance *= 2 * PI / SAMPLE_COUNT;
    
    return float4(irradiance, 1.0f);
}