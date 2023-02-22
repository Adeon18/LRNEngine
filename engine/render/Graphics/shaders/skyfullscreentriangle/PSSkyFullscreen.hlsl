#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float3 direction : DIR;
};

TextureCube textureCube: register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return textureCube.Sample(g_linearWrap, inp.direction);
}