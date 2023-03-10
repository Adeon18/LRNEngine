#include "../globals.hlsli"

struct VS_OUTPUT
{
    float4 clipPos : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

TextureCube textureCube : register(t0);

float4 main(VS_OUTPUT inp) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}