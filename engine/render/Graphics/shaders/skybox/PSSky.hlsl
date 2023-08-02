#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 modelPos : POS;
};

TextureCube textureCube;

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return textureCube.Sample(g_anisotropicWrap, inp.modelPos);
}